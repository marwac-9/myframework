#include "PhysicsManager.h"
#include "Material.h"
#include "Node.h"
#include "DebugDraw.h"
#include <algorithm>
#include <chrono>
#include "Line.h"
#include "Point.h"
#include "GraphicsStorage.h"

using namespace std;

PhysicsManager::PhysicsManager()
{
	defObbColor = glm::vec3(0.f, 0.8f, 0.8f);
	defAabbColor = glm::vec3(1.f, 0.54f, 0.f);
	GraphicsStorage::assetRegistry.RegisterType<ObjectPoint>();
}

PhysicsManager::~PhysicsManager()
{
}

PhysicsManager* PhysicsManager::Instance()
{
	static PhysicsManager instance;

	return &instance;
}

//this is process for face contacts using vector<Vector3> contacts
void PhysicsManager::ProcessContact(const Contact& contact, glm::vec3&vel1, glm::vec3& ang_vel1, glm::vec3&vel2, glm::vec3& ang_vel2, double dtInv)
{
	//BAUMGARTE
	double bias = -BAUMGARTE * dtInv * std::min(0.0, -contact.penetration + k_allowedPenetration);

	//compute impulse force
	RigidBody* ent1 = contact.one;
	RigidBody* ent2 = contact.two;

	const double e = std::min(ent1->restitution, ent2->restitution);//0.0f; //restitution, if set to 0 then the object we collide with will absorb most of the impact and won't move much, if set to 1 both objects will fly their way
	const glm::vec3& rA = contact.contactPoint - ent1->object->bounds->centeredPosition;
	const glm::vec3& rB = contact.contactPoint - ent2->object->bounds->centeredPosition;
	const glm::vec3& kA = glm::cross(rA, contact.contactNormal);
	const glm::vec3& kB = glm::cross(rB, contact.contactNormal);

	const glm::vec3& uA = ent1->inverse_inertia_tensor_world*kA;
	const glm::vec3& uB = ent2->inverse_inertia_tensor_world*kB;

	// Precompute normal mass, tangent mass, and bias.

	// Normal Mass //is it force of normal pushing away from the surface?
	double normalMass = ent1->massInverse + ent2->massInverse;

	//if (!ent1->isKinematic)	{
		normalMass += glm::dot(kA, uA);
	//}
	//if (!ent2->isKinematic)	{
		normalMass += glm::dot(kB, uB);
	//}

	//problem lies in the kinematic object to have zero inertia tensor
	//relative velocity then is not changed
	//but the denominator or normal mass will be too low 

	// Relative velocity at contact
	const glm::vec3& relativeVel = (ent2->velocity + glm::cross(ent2->angular_velocity, rB)) - (ent1->velocity + glm::cross(ent1->angular_velocity, rA));
	double numer = -(1.0 + e)* glm::dot(relativeVel, contact.contactNormal) + bias;
	//double denom = ent1->massInverse + ent2->massInverse + (ent1->inverse_inertia_tensor_world*kA.crossProd(rA) + ent2->inverse_inertia_tensor_world*kB.crossProd(rB)).dot(contactNormal);
	//double f = numer / denom;
	float f = numer / normalMass;
	f = std::max(f, 0.0f);

	const glm::vec3& impulse = f*contact.contactNormal;
	//printf("\nimpulse: %f", f);
	
	vel1 -= impulse * (float)ent1->massInverse;
	ang_vel1 -= f*uA;

	vel2 += impulse * (float)ent2->massInverse;
	ang_vel2 += f*uB;
	
	/*
	if (DebugDraw::Instance()->debug)
	{
		//impuls normal
		DebugDraw::Instance()->line.mat->SetColor(0.f, 1.f, 1.f);
		DebugDraw::Instance()->point.mat->SetColor(0.f, 1.f, 1.f);
		DebugDraw::Instance()->DrawNormal(contactNormal, ent2->object->node->position, 6.f); //since it's ent2 which gets +impulse
	}
	*/
}

void PhysicsManager::SortAxis(std::vector<ObjectPoint*>& axisList, axis axis)
{

	//Each Object contains min and max values
	//Each Object is added to the list two times and one of them is marked as min and the other one as max
	//each time we get value we test if the we want to get it from max or min vectors
	//We want the min and max values to be updated from objects and we don't want our sort and sweep to affect them or their order
	//therefore we store pointers to the objects that contain updated values
	//and so we sort the structs with pointers istead but use the values of objects min and max to sort
	
	
	//we want our fullOverlaps hash set to contain only
	int axisListLen = axisList.size();
	for (int j = 1; j < axisListLen; j++)
	{
		ObjectPoint* currentObject = axisList[j];

		int i = j - 1;
		while (i >= 0 && axisList[i]->value(axis) > currentObject->value(axis)) // we check if values ale bigger or smaller
		{
			ObjectPoint* objToSwap = axisList[i];

			if (currentObject->isMin && !objToSwap->isMin) //penetration
			{
				//OverlapPair op = OverlapPair(objToSwap->body, currentObject->body);
				//auto search = fullOverlaps.find(op);
				//if (search == fullOverlaps.end()) {
					if (!objToSwap->body->GetIsKinematic() || !currentObject->body->GetIsKinematic())
					{
						if (CheckBoundingBoxes(objToSwap->body, currentObject->body))
						{
							//fullOverlaps.insert(op);
							fullOverlaps.insert(OverlapPair(objToSwap->body, currentObject->body));
						}
					}
				//}
			}

			else if (!currentObject->isMin && objToSwap->isMin) //separation
			{
				fullOverlaps.erase(OverlapPair(objToSwap->body, currentObject->body));
				//satOverlaps.erase(OverlapPair(objToSwap->body, currentObject->body));
				//currentObject->body->aabb.color = defAabbColor;
				//objToSwap->body->aabb.color = defAabbColor;
			}

			axisList[i + 1] = objToSwap;
			i = i - 1;
		}
		axisList[i + 1] = currentObject;
	}
}

bool PhysicsManager::CheckBoundingBoxes(RigidBody* body1, RigidBody* body2)
{
	MinMax& box1 = body1->object->bounds->obb.mm;
	MinMax& box2 = body2->object->bounds->obb.mm;

	return ((
		((box1.max.z >= box2.min.z) && (box1.min.z <= box2.max.z)) &&
		((box1.max.y >= box2.min.y) && (box1.min.y <= box2.max.y))) &&
		((box1.max.x >= box2.min.x) && (box1.min.x <= box2.max.x)));
}

void PhysicsManager::RegisterRigidBody(RigidBody* body)
{	
	ObjectPoint* objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = true;
	xAxis.push_back(objectPoint); //min

	objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = false;
	xAxis.push_back(objectPoint); //max


	objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = true;
	yAxis.push_back(objectPoint); 

	objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = false;
	yAxis.push_back(objectPoint);


	objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = true;
	zAxis.push_back(objectPoint); 

	objectPoint = GraphicsStorage::assetRegistry.AllocAsset<ObjectPoint>();
	objectPoint->body = body;
	objectPoint->isMin = false;
	zAxis.push_back(objectPoint);
}

void  PhysicsManager::SortAndSweep()
{

	SortAxis(xAxis, x);
	SortAxis(yAxis, y);
	SortAxis(zAxis, z);
	//printf("\nBroad: number of overlaps: %d\n", fullOverlaps.size());
}

#define TEST_OVERLAP(axis, index) overlapOnAxis(oneObj, twoObj, (axis), (index), axisNumRes, toCentre, smallestPenetration, smallestAxis)
bool PhysicsManager::IntersectionTest(const RigidBody* oneObj, const RigidBody* twoObj, double& smallestPenetration, glm::vec3& smallestAxis, glm::vec3& toCentre, int& axisNumRes, int& bestSingleAxis)
{
	smallestPenetration = FLT_MAX;
	// Find the vector between the two centres
	toCentre = twoObj->object->bounds->centeredPosition - oneObj->object->bounds->centeredPosition;
	
	const glm::vec3& oneRight = MathUtils::GetAxis(oneObj->object->bounds->obb.rot, 0);
	const glm::vec3& oneUp = MathUtils::GetAxis(oneObj->object->bounds->obb.rot, 1);
	const glm::vec3& oneBack = MathUtils::GetAxis(oneObj->object->bounds->obb.rot, 2);

	const glm::vec3& twoRight = MathUtils::GetAxis(twoObj->object->bounds->obb.rot, 0);
	const glm::vec3& twoUp = MathUtils::GetAxis(twoObj->object->bounds->obb.rot, 1);
	const glm::vec3& twoBack = MathUtils::GetAxis(twoObj->object->bounds->obb.rot, 2);

	// Check on box one's axes first
	if (TEST_OVERLAP(oneRight, 0) &&
		TEST_OVERLAP(oneUp, 1) &&
		TEST_OVERLAP(oneBack, 2) &&
		// And on two's
		TEST_OVERLAP(twoRight, 3) &&
		TEST_OVERLAP(twoUp, 4) &&
		TEST_OVERLAP(twoBack, 5))
	{
		bestSingleAxis = axisNumRes;
		if (// Now on the cross products
			TEST_OVERLAP(glm::cross(oneRight, twoRight), 6) &&
			TEST_OVERLAP(glm::cross(oneRight, twoUp), 7) &&
			TEST_OVERLAP(glm::cross(oneRight, twoBack), 8) &&

			TEST_OVERLAP(glm::cross(oneUp, twoRight), 9) &&
			TEST_OVERLAP(glm::cross(oneUp, twoUp), 10) &&
			TEST_OVERLAP(glm::cross(oneUp, twoBack), 11) &&

			TEST_OVERLAP(glm::cross(oneBack, twoRight), 12) &&
			TEST_OVERLAP(glm::cross(oneBack, twoUp), 13) &&
			TEST_OVERLAP(glm::cross(oneBack, twoBack), 14)
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
#undef TEST_OVERLAP

void PhysicsManager::GenerateContacts(glm::vec3& MTV, const double& penetration, glm::vec3& toCentre, RigidBody* oneObj, RigidBody* twoObj, int axisNumRes, int& bestSingleAxis)
{
	if (axisNumRes < 3)
	{
		GenerateContactPointToFace(toCentre, MTV, penetration, oneObj, twoObj, axisNumRes);
	}
	else if (axisNumRes < 6)
	{
		GenerateContactPointToFace(-1.0f*toCentre, MTV, penetration, twoObj, oneObj, axisNumRes - 3);
	}
	else
	{
		GenerateContactEdgeToEdge(toCentre, MTV, penetration, oneObj, twoObj, axisNumRes, bestSingleAxis);
	}
}

void PhysicsManager::NarrowTestSAT(double dtInv)
{
	//since broad phase is first and sat is dependent on it
	//if i remove something from my fullOverlaps in broad phase 
	//next time sat will skip to check that separation 

	//also there are cases where aabb is still colliding but obb not
	//therefore i need to handle removal of the non colliding obbs by sat here too

	//printf("\nnumber of AABB overlaps: %d", fullOverlaps.size());
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	iterCount = fullOverlaps.size();
	for (auto& pair : fullOverlaps)
	{
		glm::vec3 MTV = glm::vec3(0.0f);
		double smallestPen = DBL_MAX;
		glm::vec3 toCentre = glm::vec3(0.0f);
		int axisNumRes = -1;
		int bestSingleAxis = -1;
		//Vector3& oneHalfSize = pair.rbody1->object->bounds->obb.halfExtents;
		//Vector3& twoHalfSize = pair.rbody2->object->bounds->obb.halfExtents;

		

		start = std::chrono::high_resolution_clock::now();
		bool intersection = IntersectionTest(pair.rbody1, pair.rbody2, smallestPen, MTV, toCentre, axisNumRes, bestSingleAxis);
		end = std::chrono::high_resolution_clock::now();

		elapsed_seconds = end - start;
		intersectionTestTime = elapsed_seconds.count();


		if (intersection)
		{
			if (!pair.rbody1->GetIsKinematic() && !pair.rbody2->GetIsKinematic())
			{
				pair.rbody1->SetAwake();
				pair.rbody2->SetAwake();
			}
			
			//generate contacts based on the result
			start = std::chrono::high_resolution_clock::now();
			GenerateContacts(MTV, smallestPen, toCentre, pair.rbody1, pair.rbody2, axisNumRes, bestSingleAxis);
			end = std::chrono::high_resolution_clock::now();

			elapsed_seconds = end - start;
			generateContactsTime = elapsed_seconds.count();
			
			//satOverlaps.insert(pair);
			glm::vec3 changeInVel1 = glm::vec3(0.0f);
			glm::vec3 changeInAng_Vel1 = glm::vec3(0.0f);
			glm::vec3 changeInVel2 = glm::vec3(0.0f);
			glm::vec3 changeInAng_Vel2 = glm::vec3(0.0f);

			start = std::chrono::high_resolution_clock::now();
			for (auto& contact : contacts)
			{
				iterCount++;
				/*
				if (DebugDraw::Instance()->debug)
				{
					//draw final contact points
					DebugDraw::Instance()->line.mat->SetColor(0, 1, 1);
					DebugDraw::Instance()->point.mat->SetColor(0, 1, 1);
					DebugDraw::Instance()->DrawNormal(contact->contactNormal, contact->contactPoint);
				}
				*/
				ProcessContact(contact, changeInVel1, changeInAng_Vel1, changeInVel2, changeInAng_Vel2, dtInv);
			}
			end = std::chrono::high_resolution_clock::now();
			elapsed_seconds = end - start;
			processContactTime = elapsed_seconds.count();

			start = std::chrono::high_resolution_clock::now();
			
			if (contacts.size() > 0) //if we have at least one contact then we add the cumulated change in velocity to both objects
			{
				Contact& contact = contacts[0];
				if (!contact.one->GetIsKinematic())
				{
					contact.one->velocity += changeInVel1;
					contact.one->angular_velocity += changeInAng_Vel1;
				}
				if (!contact.two->GetIsKinematic())
				{
					contact.two->velocity += changeInVel2;
					contact.two->angular_velocity += changeInAng_Vel2;
				}
				PositionalCorrection(contact.one, contact.two, smallestPen, contact.contactNormal);

				contacts.clear();
			}
			end = std::chrono::high_resolution_clock::now();

			elapsed_seconds = end - start;
			positionalCorrectionTime = elapsed_seconds.count();
		}
		else
		{
			//pair.rbody1->aabb.color = defAabbColor;
			//pair.rbody2->aabb.color = defAabbColor;
			//satOverlaps.erase(pair);
		}
	}
	//printf("\nnumber of SAT overlaps: %d", satOverlaps.size());
}

void PhysicsManager::GenerateContactPointToFace(
	const glm::vec3&toCentre,
	glm::vec3& mtv,
	double smallestPen,
	RigidBody* oneObj, RigidBody* twoObj,
	int typeOfCollision)
{
	glm::vec3 normal1 = glm::vec3(), normal2 = glm::vec3();
	double neg_offset1 = 0.0, pos_offset1 = 0.0, neg_offset2 = 0.0, pos_offset2 = 0.0;

	CreateSidePlanesOffsetsAndNormals(oneObj->object->bounds->centeredPosition, typeOfCollision, normal1, oneObj->object->bounds->obb.rot, normal2, neg_offset1, oneObj->object->bounds->obb.halfExtents, pos_offset1, neg_offset2, pos_offset2);
	
	FlipMTVTest(mtv, toCentre); //this will calculate correct reference normal pointing from box one to box two just like for edge to edge
	glm::vec3 incident_axis = glm::vec3(); //correct incident axis is not the mtv it's the most opposite axis to the mtv(in fact reference normal) of the object that collided with face 
	ClaculateIncidentAxis(incident_axis, twoObj->object->bounds->obb.rot, mtv);

	//plane is 
	//normal and point on plane
	//all points on plane are described with
	//normal dot all points - normal dot point on plane
	//we can write
	//normal dot (any point - point on plane)
	//or 
	//any point -> B
	//point on plane -> A
	//AB = B-A
	//normal dot AB = 0
	//This expression can be written as normal dot any point equals d
	//where d = normal dot point on plane, d is then interpreted as a signed distance
	//if point is on plane then
	//normal dot any point - d = 0
	//so it's (normal dot any point) - (normal dot point on plane) = 0 if object is on plane, - if behind, + if in front
	//When a normalized plane equation is evaluated for a given point, the
	//obtained result is the signed distance of the point from the plane(negative if the
	//point is behind the plane, otherwise positive).

	//so d is the distance of the plane from the origin if d is described with point on plane
	//we could precompute the plane and use the d in such manner:
	//normal dot any point - d = 0 if the any point is on plane
	//in example case normal is 0 1 0
	//we can assume that point 4 1 2 is on plane
	//d results in 1
	//then we test if any point 0 -3 0 is on plane
	//we get -3 now is it on? below? or above?
	//-3 - 1 = -4 -> below
	//if point is 0 3 0
	//we get 3
	//3 - 1 = 2 -> above plane

	//Vector3 normTest(0.0,1.0,0.0);
	//Vector3 pointOnPlane(4.0,0.0,2.0); //(4,0,2) has dist 0 while (4,1,2) has dist 1 and (4,-1,2) has -1 dist
	//Vector3 pointTest(0.0, -3.0, 0.0);
	//double signedDist = normTest.dot(pointTest);
	//double signedDist2 = normTest.dot(pointOnPlane);
	
	glm::vec3 incident_face[4];
	glm::vec3 reference_face[4];
	CalcFaceVertices(twoObj->object->bounds->centeredPosition, incident_face, incident_axis, twoObj->object->bounds->obb.rot, twoObj->object->bounds->obb.halfExtents);
	CalcFaceVertices(oneObj->object->bounds->centeredPosition, reference_face, mtv, oneObj->object->bounds->obb.rot, oneObj->object->bounds->obb.halfExtents);

	//size_t vertCount = 0;
	const glm::vec3& normal1Neg = -1.0f*normal1;
	const glm::vec3& normal2Neg = -1.0f*normal2;

	clipPolygon.clear();

	clipPolygon.push_back(incident_face[0]);
	clipPolygon.push_back(incident_face[1]);
	clipPolygon.push_back(incident_face[2]);
	clipPolygon.push_back(incident_face[3]);

	//incident face is sent then
	ClipFaceToSidePlane(clipPolygon, newClipPolygon, normal1Neg, neg_offset1); //red
	//if (DebugDraw::Instance()->debug) vertCount = DrawPlaneClipContacts(contacts, -1.f*normal1, vertCount, Vector3(1, 0, 0));
	ClipFaceToSidePlane(newClipPolygon, clipPolygon, normal1, pos_offset1); //green
	//if (DebugDraw::Instance()->debug) vertCount = DrawPlaneClipContacts(contacts, normal1, vertCount, Vector3(0, 1, 0));
	ClipFaceToSidePlane(clipPolygon, newClipPolygon, normal2Neg, neg_offset2); //blue
	//if (DebugDraw::Instance()->debug) vertCount = DrawPlaneClipContacts(contacts, -1.f*normal2, vertCount, Vector3(0, 0, 1));
	ClipFaceToSidePlane(newClipPolygon, clipPolygon, normal2, pos_offset2); //yellow
	//if (DebugDraw::Instance()->debug) vertCount = DrawPlaneClipContacts(contacts, normal2, vertCount, Vector3(1, 1, 0));

	//ref normal is mtv
	double refPlaneOffset = glm::dot(oneObj->object->bounds->centeredPosition, mtv);
	if (typeOfCollision == -1)
	{
		printf("wth\n");
	}
	refPlaneOffset = refPlaneOffset + oneObj->object->bounds->obb.halfExtents[typeOfCollision];

	//slower more logical way
	//Vector3 centerPointOfRefFace = oneObj->node->position + mtv*oneObj->obb.halfExtent[typeOfCollision];
	//float pos_offsettT = centerPointOfRefFace.dot(mtv); 

	FilterContactsAgainstReferenceFace(mtv, refPlaneOffset, smallestPen, oneObj, twoObj);
	//if (DebugDraw::Instance()->debug) DrawFaceDebug(contact_points_out, reference_face, incident_face, typeOfCollision);
}

void PhysicsManager::GenerateContactEdgeToEdge(const glm::vec3&toCentre, glm::vec3& mtv, double smallestPen, RigidBody* oneObj, RigidBody* twoObj, int axisNumRes, int& bestSingleAxis)
{
	// We've got an edge-edge contact. Find out which axes
	
	axisNumRes -= 6;
	unsigned oneAxisIndex = axisNumRes / 3;
	unsigned twoAxisIndex = axisNumRes % 3;
	const glm::vec3& oneAxis = MathUtils::GetAxis(oneObj->object->bounds->obb.rot, oneAxisIndex);
	const glm::vec3& twoAxis = MathUtils::GetAxis(twoObj->object->bounds->obb.rot, twoAxisIndex);

	// The axis should point from box one to box two.
	double toCentreDist = glm::dot(mtv, toCentre);
	if (toCentreDist > 0) //have turned sign!
	{
		mtv = mtv * -1.0f;
	}

	// We have the axes, but not the edges: each axis has 4 edges parallel
	// to it, we need to find which of the 4 for each object. We do
	// that by finding the point in the centre of the edge. We know
	// its component in the direction of the box's collision axis is zero
	// (its a mid-point) and we determine which of the extremes in each
	// of the other axes is closest.
	glm::vec3 ptOnOneEdge = oneObj->object->bounds->obb.halfExtents;
	glm::vec3 ptOnTwoEdge = twoObj->object->bounds->obb.halfExtents;
	for (int i = 0; i < 3; i++)
	{
		if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
		else if (glm::dot(MathUtils::GetAxis(oneObj->object->bounds->obb.rot, i), mtv) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

		if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
		else if (glm::dot(MathUtils::GetAxis(twoObj->object->bounds->obb.rot, i), mtv) < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
	}

	// Move them into world coordinates (they are already oriented
	// correctly, since they have been derived from the axes).
	ptOnOneEdge = oneObj->object->bounds->obb.rot * ptOnOneEdge + oneObj->object->bounds->centeredPosition;
	ptOnTwoEdge = twoObj->object->bounds->obb.rot * ptOnTwoEdge + twoObj->object->bounds->centeredPosition;

	// So we have a point and a direction for the colliding edges.
	// We need to find out point of closest approach of the two
	// line-segments.

	const glm::vec3& vertex = contactPoint(
		ptOnOneEdge, oneAxis, oneObj->object->bounds->obb.halfExtents[oneAxisIndex],
		ptOnTwoEdge, twoAxis, twoObj->object->bounds->obb.halfExtents[twoAxisIndex],
		bestSingleAxis > 2
		);

	// We can fill the contact.
	//Contact contact;
	//contact.penetration = smallestPen;
	//contact.contactNormal = mtv;
	//contact.contactPoint = vertex;
	//contact.one = twoObj;
	//contact.two = oneObj;

	contacts.emplace_back(vertex, mtv, smallestPen, twoObj, oneObj);
}

void PhysicsManager::PositionalCorrection(RigidBody* one, RigidBody* two, double penetration, glm::vec3& normal)
{
	const float percent = 0.2; // usually 20% to 80%
	const double slop = 0.01; // usually 0.01 to 0.1
	//Vector3 correction = (std::max(penetration - slop, 0.0f) / (one->massInverse + two->massInverse)) * normal;
	glm::vec3 correction = (float)(std::max(penetration - slop, 0.0) / (one->massInverse + two->massInverse)) * percent * normal;
	one->object->node->Translate(-1.0f * (float)one->massInverse * correction);
	two->object->node->Translate((float)two->massInverse * correction);
}

void PhysicsManager::PositionalImpulseCorrection(RigidBody* one, RigidBody* two, Contact& contact)
{
	const double percent = 1.0; // usually 20% to 80%
	const double slop = 0.01; // usually 0.01 to 0.1
	//Vector3 correction = (std::max(penetration - slop, 0.0f) / (one->massInverse + two->massInverse)) * normal;
	double mag = (std::max(contact.penetration - slop, 0.0) / (one->massInverse + two->massInverse)) * percent;
	one->ApplyImpulse(-1.0f * contact.contactNormal, one->massInverse * mag, contact.contactPoint);
	two->ApplyImpulse(contact.contactNormal, two->massInverse * mag, contact.contactPoint);
}

void PhysicsManager::CalcFaceVertices(const glm::vec3& pos, glm::vec3* vertices, const glm::vec3& axis, const glm::mat3& model, const glm::vec3& halfExtents, bool counterClockwise)
{
	glm::vec3 xAxis = MathUtils::GetAxis(model, 0);
	glm::vec3 yAxis = MathUtils::GetAxis(model, 1);
	glm::vec3 zAxis = MathUtils::GetAxis(model, 2);
	glm::vec3 referencePos = glm::vec3(0.0f);;
	glm::vec3 u = glm::vec3(0.0f);;
	glm::vec3 v = glm::vec3(0.0f);;

	double sign = 0;
	if (abs(sign = glm::dot(axis, xAxis)) > 0.9)
	{
		if (sign > 0.0) referencePos = pos + halfExtents.x*xAxis;
		else referencePos = pos - halfExtents.x*xAxis;
		u = halfExtents.y*yAxis;
		v = halfExtents.z*zAxis;
	}
	else if (abs(sign = glm::dot(axis, yAxis)) > 0.9)
	{
		if (sign > 0.0) referencePos = pos + halfExtents.y*yAxis;
		else referencePos = pos - halfExtents.y*yAxis;
		u = halfExtents.x*xAxis;
		v = halfExtents.z*zAxis;
	}
	else if (abs(sign = glm::dot(axis, zAxis)) > 0.9)
	{
		if (sign > 0.0)	referencePos = pos + halfExtents.z*zAxis;
		else referencePos = pos - halfExtents.z*zAxis;
		u = halfExtents.x*xAxis;
		v = halfExtents.y*yAxis;
	}

	if (counterClockwise) {
		vertices[0] = referencePos + u + v;
		vertices[1] = referencePos - u + v;
		vertices[2] = referencePos - u - v;
		vertices[3] = referencePos + u - v;
	} else {
		vertices[0] = referencePos + u + v;
		vertices[1] = referencePos + u - v;
		vertices[2] = referencePos - u - v;
		vertices[3] = referencePos - u + v;
	}
}

void PhysicsManager::ClipFaceToSidePlane(vector<glm::vec3>& clipPolygon, std::vector<glm::vec3>& newClipPolygon, const glm::vec3& normal, double plane_offset)
{
	glm::vec3 Vertex1 = clipPolygon.back();
	double Distance1 = glm::dot(Vertex1, normal) - plane_offset;//rnDistance(Plane, Vertex1.Position);
	newClipPolygon.clear();
	for (auto & Vertex2 : clipPolygon)
	{
		iterCount++;
		double Distance2 = glm::dot(Vertex2, normal) - plane_offset;//rnDistance(Plane, Vertex2.Position);

		if (Distance1 <= 0.0 && Distance2 <= 0.0)
		{
			// Both vertices are behind the plane - keep vertex2
			newClipPolygon.push_back(Vertex2);
		}
		else if (Distance1 <= 0.0 && Distance2 > 0.0)
		{
			// Vertex1 is behind the plane, vertex2 is in front -> intersection point
			float Fraction = Distance1 / (Distance1 - Distance2);
			const glm::vec3& newVertex = Vertex1 + Fraction * (Vertex2 - Vertex1);

			// Keep intersection point 
			newClipPolygon.push_back(newVertex);
		}
		else if (Distance2 <= 0.0 && Distance1 > 0.0)
		{
			// Vertex2 is behind the plane, vertex1 is in front -> intersection point
			float Fraction = Distance1 / (Distance1 - Distance2);
			const glm::vec3& newVertex1 = Vertex1 + Fraction * (Vertex2 - Vertex1);

			// Keep intersection point 
			newClipPolygon.push_back(newVertex1);

			// And also keep vertex2
			newClipPolygon.push_back(Vertex2);
		}

		// Keep vertex2 as starting vertex for next edge
		Vertex1 = Vertex2;
		Distance1 = Distance2;
	}
}

inline void PhysicsManager::CreateSidePlanesOffsetsAndNormals(const glm::vec3& onePosition, int typeOfCollision, glm::vec3&normal1, const glm::mat3 &one, glm::vec3&normal2, double &neg_offset1, glm::vec3 oneHalfSize, double &pos_offset1, double &neg_offset2, double &pos_offset2)
{
	//get normal for all face and it's offset
	switch (typeOfCollision)
	{
	case 0: //skip x
	{
		//we get y and z axis
		normal1 = MathUtils::GetAxis(one, 1);
		normal2 = MathUtils::GetAxis(one, 2);
		double y = glm::dot(onePosition, normal1);  //returns distance from origo in normal1 direction
		double z = glm::dot(onePosition, normal2);
		neg_offset1 = -y + oneHalfSize[1];
		//float myNegOff = (onePosition - normal1*oneHalfSize[1]).dot(-1.f*normal1); //now that's the correct distance of the plane from origo we must use correct oriented normal when dotting
		pos_offset1 = y + oneHalfSize[1];
		neg_offset2 = -z + oneHalfSize[2];
		pos_offset2 = z + oneHalfSize[2];
		/*
		//so that hax method above works below is more logical way
		neg_offset1 = (onePosition - normal1*oneHalfSize[1]).dot(-1.f*normal1); 
		pos_offset1 = (onePosition + normal1*oneHalfSize[1]).dot(normal1);
		neg_offset2 = (onePosition - normal2*oneHalfSize[2]).dot(-1.f*normal2);
		pos_offset2 = (onePosition + normal2*oneHalfSize[2]).dot(normal2);
		*/

		//if (DebugDraw::Instance()->debug) DrawSidePlanes(normal1, normal2, onePosition, 1, 2, oneHalfSize);	
	}
	break;

	case 1: //skip y
	{
		//we get x and z axis
		normal1 = MathUtils::GetAxis(one, 0);
		normal2 = MathUtils::GetAxis(one, 2);
		double x = glm::dot(onePosition, normal1);
		double z = glm::dot(onePosition, normal2);
		neg_offset1 = -x + oneHalfSize[0];
		pos_offset1 = x + oneHalfSize[0];
		neg_offset2 = -z + oneHalfSize[2];
		pos_offset2 = z + oneHalfSize[2];

		//if (DebugDraw::Instance()->debug) DrawSidePlanes(normal1, normal2, onePosition, 0, 2, oneHalfSize);
	}
	break;

	case 2: //skip z
	{
		//we get x and y axis
		normal1 = MathUtils::GetAxis(one, 0);
		normal2 = MathUtils::GetAxis(one, 1);
		double x = glm::dot(onePosition, normal1);
		double y = glm::dot(onePosition, normal2);
		neg_offset1 = -x + oneHalfSize[0];
		pos_offset1 = x + oneHalfSize[0];
		neg_offset2 = -y + oneHalfSize[1];
		pos_offset2 = y + oneHalfSize[1];

		//if (DebugDraw::Instance()->debug) DrawSidePlanes(normal1, normal2, onePosition, 0, 1, oneHalfSize);
	}
	break;
	}
}

inline void PhysicsManager::ClaculateIncidentAxis(glm::vec3& incident_axis, const glm::mat3 &two, glm::vec3& smallestAxis)
{
	double incident_tracker = DBL_MAX;
	//get incident face vertices by dotting all axis with mtv
	for (int i = 0; i < 3; i++)
	{
		iterCount++;
		double most_neg = glm::dot(MathUtils::GetAxis(two, i), smallestAxis);
		if (most_neg < incident_tracker){
			incident_tracker = most_neg;
			incident_axis = MathUtils::GetAxis(two, i);
		}
		most_neg = (-1.0f * glm::dot(MathUtils::GetAxis(two, i), smallestAxis));
		if (most_neg < incident_tracker){
			incident_tracker = most_neg;
			incident_axis = -1.0f * MathUtils::GetAxis(two, i);
		}
	}
}

inline void PhysicsManager::FilterContactsAgainstReferenceFace(const glm::vec3& refNormal, double pos_offsett, double penetration, RigidBody* one, RigidBody* two)
{
	//this step is supposed to test against the reference plane all the contact points and we keep only the points inside or at the cube
	//this is to clean up contacts 
	for (auto & contactPoint : clipPolygon)
	{
		iterCount++;
		//float Distance1 = Vector3::Dot(contacts[i], -1 * refNormal) - neg_offsett;
		//if (Distance1 <= 0){
		//	//keep point
		//	contact_points_out.push_back(contacts[i]);
		//}
		double Distance2 = pos_offsett - glm::dot(contactPoint, refNormal);
		if (Distance2 >= 0.0){
			//keep point
			//Contact contact;
			//contact.contactPoint = contactPoint;
			//contact.penetration = Distance2; //penetration per contact
			////contact->penetration = penetration; //mtv penetration
			//contact.contactNormal = refNormal;
			//contact.one = one;
			//contact.two = two;
			contacts.emplace_back(contactPoint, refNormal, Distance2, one, two);
		}
	}
}

inline void PhysicsManager::FlipMTVTest(glm::vec3&mtv, const glm::vec3& toCentre)
{
	// We know which axis the collision is on (i.e. best),
	// but we need to work out which of the two faces on
	// this axis. //flipping
	double toCentreDist = glm::dot(mtv, toCentre);
	if (toCentreDist < 0) //have turned sign!
	{
		mtv = mtv * -1.0f;
	}
}

void PhysicsManager::DrawSidePlanes(const glm::vec3& normal1, const glm::vec3& normal2, const glm::vec3& onePosition, int index1, int index2, const glm::vec3& oneHalfSize)
{
	const glm::vec3& vertexOnPlane1 = onePosition - normal1*oneHalfSize[index1]; // -1*normal
	const glm::vec3& vertexOnPlane2 = onePosition + normal1*oneHalfSize[index1];
	const glm::vec3& vertexOnPlane3 = onePosition - normal2*oneHalfSize[index2]; // -1*normal
	const glm::vec3& vertexOnPlane4 = onePosition + normal2*oneHalfSize[index2];
	//let's draw calculated planes with normals at plane points
	//Line::Instance()->mat->SetColor(1, 0, 0); //red
	//Point::Instance()->mat->SetColor(1, 0, 1);
	DebugDraw::Instance()->DrawPlaneN(-1.0f*normal1, vertexOnPlane1);

	//Line::Instance()->mat->SetColor(0, 1, 0); //green
	//Point::Instance()->mat->SetColor(1, 0, 1);
	DebugDraw::Instance()->DrawPlaneN(normal1, vertexOnPlane2);

	//Line::Instance()->mat->SetColor(0, 0, 1); //blue
	//Point::Instance()->mat->SetColor(1, 0, 1);
	DebugDraw::Instance()->DrawPlaneN(-1.0f*normal2, vertexOnPlane3);

	//Line::Instance()->mat->SetColor(1, 1, 0); //yellow
	//Point::Instance()->mat->SetColor(1, 0, 1);
	DebugDraw::Instance()->DrawPlaneN(normal2, vertexOnPlane4);
}

void PhysicsManager::DrawReferenceAndIncidentFace(glm::vec3* reference_face, glm::vec3* incident_face)
{
	//reference face verts
	//Point::Instance()->mat->SetColor(0, 1, 1);
	DebugDraw::Instance()->DrawPoint(reference_face[0], 15);
	DebugDraw::Instance()->DrawPoint(reference_face[1], 15);
	DebugDraw::Instance()->DrawPoint(reference_face[2], 15);
	DebugDraw::Instance()->DrawPoint(reference_face[3], 15);
	
	//draw incident face verts
	//Point::Instance()->mat->SetColor(1, 0, 0);
	DebugDraw::Instance()->DrawPoint(incident_face[0], 15); //upper left
	DebugDraw::Instance()->DrawPoint(incident_face[1], 15); //lower left
	DebugDraw::Instance()->DrawPoint(incident_face[2], 15); //lower right
	DebugDraw::Instance()->DrawPoint(incident_face[3], 15); //upper right
}

void PhysicsManager::DrawCollisionNormal(Contact& contact)
{
	//collision normal
	//Line::Instance()->mat->SetColor(0, 0, 0); //black 
	//Point::Instance()->mat->SetColor(0, 0, 0);
	DebugDraw::Instance()->DrawNormal(contact.contactNormal, contact.two->object->bounds->centeredPosition); //draw mtv after flip
}

void PhysicsManager::DrawReferenceNormal(Contact& contact, int typeOfCollision)
{
	//reference normal
	glm::vec3 centerPointOfRefFace = contact.one->object->bounds->centeredPosition + contact.contactNormal*contact.one->object->bounds->obb.halfExtents[typeOfCollision];
	//Point::Instance()->mat->SetColor(1, 1, 1); //white
	//Line::Instance()->mat->SetColor(1, 1, 1);
	DebugDraw::Instance()->DrawNormal(contact.contactNormal, centerPointOfRefFace); //draw reference normal
}

void PhysicsManager::DrawFaceDebug(glm::vec3* reference_face, glm::vec3* incident_face, int typeOfCollision)
{
	if (contacts.size() > 0)
	{
		DrawCollisionNormal(contacts[0]);
		DrawReferenceNormal(contacts[0], typeOfCollision);
		DrawReferenceAndIncidentFace(reference_face, incident_face);
	}
}

size_t PhysicsManager::DrawPlaneClipContacts(std::vector<glm::vec3> &contactPoints, const glm::vec3& normal, size_t vertCount, const glm::vec3& normalColor)
{
	for (size_t i = vertCount; i < contacts.size(); i++)
	{
		//Point::Instance()->mat->SetColor(1, 0, 1);
		//Line::Instance()->mat->SetColor(normalColor);
		DebugDraw::Instance()->DrawNormal(normal, contactPoints[i]);
	}	
	vertCount = contactPoints.size();
	return vertCount;
}

void PhysicsManager::Clear()
{
	/*
	for (size_t i = 0; i < xAxis.size(); i++)
	{
		delete xAxis[i];
		delete yAxis[i];
		delete zAxis[i];
	}
	*/
	GraphicsStorage::assetRegistry.ClearType<ObjectPoint>();
	xAxis.clear();
	yAxis.clear();
	zAxis.clear();
	satOverlaps.clear();
	fullOverlaps.clear();
	gravity = glm::vec3(0.0, -9.0, 0.0);
	contacts.clear();
	clipPolygon.clear();
	newClipPolygon.clear();
}

void PhysicsManager::Update(double deltaTime)
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;

	start = std::chrono::high_resolution_clock::now();
	SortAndSweep();
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	pruneAndSweepTime = elapsed_seconds.count();

	start = std::chrono::high_resolution_clock::now();
	NarrowTestSAT(deltaTime);
	end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	satTime = elapsed_seconds.count();
}
