#include "ofApp.h"

#include "Boid.h"
#include "Kinematic.h"
#include "Steering/BasicKinematicSteering.h"
#include "Steering/SeekSteering.h"
#include "Steering/WanderSteering.h"
#include "Steering/ComplexSteering/SeekArrive.h"
#include "Steering/AlignSteering.h"
#include "Steering/ComplexSteering/Flocking.h"
#include "Scene.h"

#include "Graph/Graph.h"
#include "Graph/NodeRecord.h"

#include <graphics/ofGraphics.h>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <exception>

namespace
{
	Graph* sGraph = nullptr;

	void ReleaseGraph()
	{
		delete sGraph;
		sGraph = nullptr;
	}

	float GetManhattanDistance(int a, int b)
	{
		return abs(sGraph->GetNode(a).x - sGraph->GetNode(b).x) + abs(sGraph->GetNode(a).y - sGraph->GetNode(b).y);
	}

	float GetEuclideanDistance(int a, int b)
	{
		return sqrtf(pow(sGraph->GetNode(a).x - sGraph->GetNode(b).x, 2) + pow(sGraph->GetNode(a).y - sGraph->GetNode(b).y, 2));
	}

	float GetRandomDistance(int a, int b)
	{
		return (float)rand() / RAND_MAX * Graph::width;
	}

	float GetZero(int a, int b)
	{
		return 0;
	}

	/* Get Random integer in [left, right) */
	int RandomRange(int left, int right)
	{
		return floor((float)rand() / RAND_MAX * (right - left - 1)) + left;
	}
}


//--------------------------------------------------------------
void ofApp::setup(){
	
	// Graph
	{
		srand(time(nullptr));

		goto SecondGraph;
		// Test first graph
		{
			sGraph = Graph::Load("data/map.txt");

			int numNode = 20;
			int total = 0;
			int totalNodesVisited = 0;
			int num = 0;
			
			// A* : Manhattan distance
			{
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetManhattanDistance);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::microseconds>(endClock - beginClock).count();
						totalNodesVisited += sGraph->GetRecord().size();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "Small map (mannually designed) by A* manhattan distance: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;

			}

			// A* : Euclidean distance
			{
				total = 0;
				num = 0;
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetEuclideanDistance);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::microseconds>(endClock - beginClock).count();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "Small map (mannually designed) by A* euclidean distance: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;
			}


			// A* : random
			{
				total = 0;
				num = 0;
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetRandomDistance);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::microseconds>(endClock - beginClock).count();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "Small map (mannually designed) by A* random: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;
			}

			// Dijstra
			{
				total = 0;
				num = 0;
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetZero);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::microseconds>(endClock - beginClock).count();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "Small map (mannually designed) by Dijstra: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;
			}

			ReleaseGraph();

		}
	
		std::cout << std::endl;

	SecondGraph:

		int numNode = 5000;
		// Test large graph
		{
			sGraph = Graph::GenerateRandomGraph(numNode);
			
			int total = 0;
			int totalNodesVisited = 0;
			int num = 0;

			// A* 
			{
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetEuclideanDistance);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::milliseconds>(endClock - beginClock).count();
						totalNodesVisited += sGraph->GetRecord().size();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "\nLarge map (generated randomly) by A*: euclidean distance: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;
			}

			// Dijstra
			{
				for (int i = 0; i < 100; i++)
				{
					auto beginClock = std::chrono::high_resolution_clock::now();
					bool result = sGraph->FindPath(RandomRange(0, numNode), RandomRange(0, numNode), GetZero);
					auto endClock = std::chrono::high_resolution_clock::now();
					if (result)
					{
						total += std::chrono::duration_cast<std::chrono::milliseconds>(endClock - beginClock).count();
						totalNodesVisited += sGraph->GetRecord().size();
						num++;
					}
					sGraph->ClearRecord();
				}
				std::cout << "\nLarge map (generated randomly) by Dijstra: \n"
					<< "Average elpased time = " << (float)total / num << " microseconds \n"
					<< "Average number of nodes visited = " << (float)totalNodesVisited / num << "\n" << std::endl;

			}

			ReleaseGraph();
		}
	

		// Setup grid
		// Load a grey image, convert it into grid

	}

	/* First Assignment
	{
		// setup basic motion
		Kinematic initalKinematic(glm::vec2(10, ofGetWindowHeight() - 10), 0, glm::vec2(100, 0), 0);
		Boid* initialBoid = new Boid(initalKinematic, nullptr, 100, 0, 0, 0, 0, 0, 0);

		BasicKinematicSteering* steering = new BasicKinematicSteering(initialBoid);
		initialBoid->mpKinematicSteering = steering;
		std::vector<Boid*> boids1({ initialBoid });

		Scene* scene = new Scene(boids1);
		mpScenes.push_back(scene);

		// setup seek and kinematic arrive
		mSeekTarget.position = glm::vec2(10, 10);
		Kinematic kinematicSeekKinematic(glm::vec2(10, 10), 0, glm::vec2(0, 0), 0);
		Boid* kinematicSeekBoid = new Boid(kinematicSeekKinematic, &mSeekTarget, 100, 0, 300, 0, 10, 0, 0);
		SeekKinematicArrive* kinematicSeekSteering = new SeekKinematicArrive(kinematicSeekBoid);
		kinematicSeekBoid->mpDynamicSteering = kinematicSeekSteering;
		KinematicAlignSteering* alignSteering = new KinematicAlignSteering(kinematicSeekBoid);
		kinematicSeekBoid->mpKinematicOrientationSteering = alignSteering;
		std::vector<Boid*> boids2({ kinematicSeekBoid });

		scene = new Scene(boids2);
		mpScenes.push_back(scene);

		// setup seek and dynamic arrive
		Kinematic dynamicSeekKinematic(glm::vec2(10, 10), 0, glm::vec2(0, 0), 0);
		Boid* dynamicSeekBoid = new Boid(dynamicSeekKinematic, &mSeekTarget, 100, 0, 300, 0, 10, 70, 0.1f);
		SeekDynamicArrive* dynamicSeekSteering = new SeekDynamicArrive(dynamicSeekBoid);
		dynamicSeekBoid->mpDynamicSteering = dynamicSeekSteering;
		alignSteering = new KinematicAlignSteering(dynamicSeekBoid);
		dynamicSeekBoid->mpKinematicOrientationSteering = alignSteering;
		std::vector<Boid*> boids3({ dynamicSeekBoid });

		scene = new Scene(boids3);
		mpScenes.push_back(scene);

		// setup kinematic wander
		Kinematic kinematicWanderKinematic(glm::vec2(300, 300), 0, glm::vec2(100, 0), 0);
		Boid* kinematicWanderBoid = new Boid(kinematicWanderKinematic, nullptr, 100, PI * 2, 100, 0, 0, 0, 0);
		KinematicWanderSteering* kinematicWanderSteering = new KinematicWanderSteering(kinematicWanderBoid);
		kinematicWanderBoid->mpKinematicSteering = kinematicWanderSteering;
		//alignSteering = new KinematicAlignSteering(kinematicWanderBoid);
		//kinematicWanderBoid->mpKinematicOrientationSteering = alignSteering;
		std::vector<Boid*> boids4({ kinematicWanderBoid });

		scene = new Scene(boids4);
		mpScenes.push_back(scene);

		// setup dynamic wander - face to where you to
		Kinematic dynamicWanderKinematic(glm::vec2(300, 300), 0, glm::vec2(100, 0), 0);
		Boid* dynamicWanderBoid1 = new Boid(dynamicWanderKinematic, nullptr, 100, PI * 3, 1000, 20, 5, 10, 0.1f, 50, 40, 1, 0, 0.1);
		DynamicWanderSteering* dynamicWanderSteering1 = new DynamicWanderSteering(dynamicWanderBoid1);
		dynamicWanderBoid1->mpDynamicSteering = dynamicWanderSteering1;
		FaceToWhereYouGo* faceWhereYouGo = new FaceToWhereYouGo(dynamicWanderBoid1);
		dynamicWanderBoid1->mpDynamicOrientationSteering = faceWhereYouGo;
		std::vector<Boid*> boids5({ dynamicWanderBoid1 });

		scene = new Scene(boids5);
		mpScenes.push_back(scene);

		// setup dynamic wander - face to target
		Kinematic dynamicWanderKinematic2(glm::vec2(300, 300), 0, glm::vec2(100, 0), 0);
		Boid* dynamicWanderBoid2 = new Boid(dynamicWanderKinematic2, nullptr, 100, PI * 2, 1000, 10, 5, 10, 0.1f, 50, 40, 1, 0.05, 0.1);
		DynamicWanderSteering* dynamicWanderSteering2 = new DynamicWanderSteering(dynamicWanderBoid2);
		dynamicWanderBoid2->mpDynamicSteering = dynamicWanderSteering2;
		FaceTarget* faceTarget = new FaceTarget(dynamicWanderBoid2);
		dynamicWanderBoid2->mpDynamicOrientationSteering = faceTarget;
		std::vector<Boid*> boids6({ dynamicWanderBoid2 });

		scene = new Scene(boids6);
		mpScenes.push_back(scene);


		// setup flocking
		Group* pGroup = new Group;
		std::vector<Boid*> boids7;

		// wander leader
		Kinematic leaderKinematic(glm::vec2(10, 10), 0, glm::vec2(0, 0), 0);
		Boid* leaderBoid = new Boid(leaderKinematic, nullptr, 100, PI * 5, 100, 0, 0, 0, 0);
		KinematicWanderSteering* leaderWander = new KinematicWanderSteering(leaderBoid);
		leaderBoid->mpKinematicSteering = leaderWander;
		//alignSteering = new KinematicAlignSteering(leaderBoid);
		//leaderBoid->mpKinematicOrientationSteering = alignSteering;
		leaderBoid->SetColor(ofColor(200, 0, 0));

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Kinematic flockingKinematic(glm::vec2(i * 20, j * 20), 0, glm::vec2(0, 0), 0);
				Boid* flockingBoid = new Boid(flockingKinematic, nullptr, 100, PI * 5, 100, 20, 10, 50, 0.01f);
				FlockingSteering* flockingSteering = new FlockingSteering(flockingBoid, 1.2f, 0.6f, 0.5f);
				flockingBoid->mpDynamicSteering = flockingSteering;
				alignSteering = new KinematicAlignSteering(flockingBoid);
				flockingBoid->mpKinematicOrientationSteering = alignSteering;
				flockingBoid->SetLeader(leaderBoid);
				boids7.push_back(flockingBoid);
			}
		}

		pGroup->mpBoids = boids7;
		for (auto pBoid : pGroup->mpBoids)
		{
			pBoid->SetGroup(pGroup);
		}

		boids7.push_back(leaderBoid);

		scene = new Scene(boids7, pGroup);
		mpScenes.push_back(scene);

	}
	*/

}

//--------------------------------------------------------------
void ofApp::update(){
	if(mSceneIndex >= 0 && mSceneIndex < mpScenes.size())
		mpScenes[mSceneIndex]->Update(ofGetLastFrameTime());
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(20);

	if (mSceneIndex >= 0 && mSceneIndex < mpScenes.size())
		mpScenes[mSceneIndex]->Draw();

}

void ofApp::exit()
{
	for (auto scene : mpScenes)
	{
		delete scene;
	}

	mpScenes.clear();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	int index = key - '1';
	if (index >= 0 && index < mpScenes.size())
	{
		mSceneIndex = index;
		mpScenes[mSceneIndex]->Reset();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mSeekTarget.position = glm::vec2(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
