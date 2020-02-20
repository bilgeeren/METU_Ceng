#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "writeOutput.h"
#include <vector>
#include <math.h>
#include <unistd.h>
using namespace std;

// TODO: Destructors
// TODO: Sleeps DONE
// TODO: Timeouts DONE
// TODO: input DONE
class miner {
	public:
		miner(unsigned int id, unsigned int lim, unsigned int max,OreType type, unsigned int interval){
			FillMinerInfo(minerInfo, id+1,type,lim,0);
			maximumProduceLimit = max;
			sem_init(&canProduce,1,lim);
			sem_init(&minerSem,1,1);
			isAvailable = true;
			_interval = interval;
		}
		~miner(){
			delete minerInfo;
		}
		MinerInfo *minerInfo = new MinerInfo;
		unsigned int maximumProduceLimit;
		sem_t canProduce;
		sem_t minerSem;
		bool isAvailable;
		unsigned int _interval;

};

class transporter {
	public:
		transporter(unsigned int id,OreType *type, unsigned int interval){
			FillTransporterInfo(transporterInfo, id+1, type);
			_interval = interval;

		}
		~transporter() {
			delete transporterInfo;
		}
		TransporterInfo *transporterInfo = new TransporterInfo;
		unsigned int _interval;

};

class smelter {
	public:
		smelter(unsigned int id,OreType type, unsigned int cap, unsigned int interval){
			FillSmelterInfo(smelterInfo,id+1,type,cap,0,(unsigned int)0);
			sem_init(&canProduce,1,0);
			sem_init(&smelterSem,1,1);
			_interval = interval;
			alreadyCounted = 0;
	
		} 
		~smelter(){
			delete smelterInfo;
		}
		SmelterInfo *smelterInfo = new SmelterInfo;
		sem_t smelterSem;
		sem_t canProduce;
		unsigned int _interval;
		unsigned int alreadyCounted;

};
class foundry {
	public:
		foundry(unsigned int id,unsigned int cap,unsigned int interval){
			FillFoundryInfo(foundryInfo,id+1,cap,0,0,(unsigned int)0);
			sem_init(&canProduce,1,0);
			sem_init(&foundrySem,1,1);
			_interval = interval;
			alreadyCounted = 0;

		} 
		~foundry(){
			delete foundryInfo;
		}
		FoundryInfo *foundryInfo = new FoundryInfo;
		sem_t foundrySem;
		sem_t canProduce;
		unsigned int _interval;
		unsigned int alreadyCounted;

};

std::vector<miner*> minersVec;
std::vector<smelter*> smeltersVec;
std::vector<foundry*> foundriesVec;
sem_t generalTransporterSem;
sem_t anyTranspoterSearching;
sem_t protectTotal;
int lastIndex;
int total;

sem_t protectIron;
sem_t protectCopper;
sem_t protectCoal;
sem_t eraseSmelter;
sem_t eraseFoundry;
bool isEmpty = false;
int countOfEreasedMiner=0;
void deleteWithId(int id){
	for (int i =0 ; i<minersVec.size();i++){
		if(minersVec[i]->minerInfo->ID == id ){
			miner *temp = minersVec[i];
			sem_destroy(&temp->minerSem);
			sem_destroy(&temp->canProduce);		
			minersVec.erase(minersVec.begin()+i);
			temp->~miner();
			break;
		}
	}
}

void *miner_thread(void *currMiner){
	miner *myMiner; 
	myMiner = (miner* ) currMiner;
	int mid = myMiner->minerInfo->ID;

	WriteOutput(myMiner->minerInfo,NULL, NULL, NULL, MINER_CREATED);
	while(myMiner->maximumProduceLimit>0){
		sem_wait(&myMiner->canProduce);
		if ( myMiner->minerInfo->current_count < myMiner->minerInfo->capacity){
			sem_wait(&myMiner->minerSem);

			WriteOutput(myMiner->minerInfo,NULL, NULL, NULL, MINER_STARTED);
			//SLEEP
			usleep(myMiner->_interval - (myMiner->_interval*0.01) + (rand()%(int)(myMiner->_interval*0.02)));
			myMiner->minerInfo->current_count++;
			myMiner->maximumProduceLimit--;
			FillMinerInfo(myMiner->minerInfo, myMiner->minerInfo->ID,myMiner->minerInfo->oreType,myMiner->minerInfo->capacity,myMiner->minerInfo->current_count);
			WriteOutput(myMiner->minerInfo,NULL, NULL, NULL, MINER_FINISHED);
			usleep(myMiner->_interval - (myMiner->_interval*0.01) + (rand()%(int)(myMiner->_interval*0.02)));
			sem_post(&myMiner->minerSem);
			sem_wait(&protectTotal);
			total++;
			sem_post(&protectTotal);
			//cout<<"***************"<<endl;
			sem_post(&generalTransporterSem);
			//cout<<"========================"<<endl;

			// SLEEP
		}
	}
	sem_wait(&myMiner->minerSem);
	FillMinerInfo(myMiner->minerInfo, myMiner->minerInfo->ID,myMiner->minerInfo->oreType,myMiner->minerInfo->capacity,myMiner->minerInfo->current_count);
	sem_post(&myMiner->minerSem);
	WriteOutput(myMiner->minerInfo,NULL, NULL, NULL, MINER_STOPPED);
	pthread_exit(NULL);
}
void *smelter_thread(void *currSmelter){
	smelter *mySmelter; 
	mySmelter = (smelter* ) currSmelter;

	for (int i=0; i<mySmelter->smelterInfo->loading_capacity;i++){
		if(mySmelter->smelterInfo->oreType== IRON){
			sem_post(&protectIron);				
		}
		else{
			sem_post(&protectCopper);			
			
		}
	}
	WriteOutput(NULL,NULL,mySmelter->smelterInfo,NULL,SMELTER_CREATED);
	while(1){
		//cout<<"-----------"<<endl;
		 struct timespec tm;
		int i = 0;
		while(sem_timedwait(&mySmelter->canProduce, &tm) == -1){
			clock_gettime(CLOCK_REALTIME, &tm);
			i++;
	        tm.tv_sec += 1;
	        if (i==5) {
	            FillSmelterInfo(mySmelter->smelterInfo,mySmelter->smelterInfo->ID,mySmelter->smelterInfo->oreType,mySmelter->smelterInfo->loading_capacity,mySmelter->smelterInfo->waiting_ore_count,mySmelter->smelterInfo->total_produce);
				WriteOutput(NULL,NULL,mySmelter->smelterInfo,NULL,SMELTER_STOPPED);
				//deleteWithIdSmelter(mySmelter->smelterInfo->ID);
				sem_destroy(&mySmelter->smelterSem);
				sem_destroy(&mySmelter->canProduce);		
				mySmelter->~smelter();
				pthread_exit(NULL);
	        }
		}
		//cout<<"-**************-"<<endl;
			WriteOutput(NULL,NULL,mySmelter->smelterInfo,NULL,SMELTER_STARTED);
			usleep(mySmelter->_interval - (mySmelter->_interval*0.01) + (rand()%(int)(mySmelter->_interval*0.02)));
			sem_wait(&mySmelter->smelterSem);
			//cout<<"-=============**********"<<endl;

			mySmelter->smelterInfo->waiting_ore_count = mySmelter->smelterInfo->waiting_ore_count -2;
			mySmelter->smelterInfo->total_produce ++;
			mySmelter->alreadyCounted -=2;
			FillSmelterInfo(mySmelter->smelterInfo,mySmelter->smelterInfo->ID,mySmelter->smelterInfo->oreType,mySmelter->smelterInfo->loading_capacity,mySmelter->smelterInfo->waiting_ore_count,mySmelter->smelterInfo->total_produce);
			WriteOutput(NULL,NULL,mySmelter->smelterInfo,NULL,SMELTER_FINISHED);
			if(mySmelter->smelterInfo->oreType== IRON){
				sem_post(&protectIron);				
				sem_post(&protectIron);
			}
			else{
				sem_post(&protectCopper);			
				sem_post(&protectCopper);
			}
				
			sem_post(&mySmelter->smelterSem);
		// __TIMEOUT CASE

		//SLEEP
	}
}	
void *foundry_thread(void *currFoundry){
	foundry *myFoundry;
	myFoundry = (foundry*) currFoundry;
	for (int i=0; i<myFoundry->foundryInfo->loading_capacity;i++){
		
		sem_post(&protectIron);
		sem_post(&protectCoal);
	}

	WriteOutput(NULL,NULL,NULL,myFoundry->foundryInfo,FOUNDRY_CREATED);
	while(1){
		 struct timespec tm;
		int i = 0;
		while(sem_timedwait(&myFoundry->canProduce, &tm) == -1){
			clock_gettime(CLOCK_REALTIME, &tm);
			i++;
	        tm.tv_sec += 1;
	        if (i==5) {
	            FillFoundryInfo(myFoundry->foundryInfo,myFoundry->foundryInfo->ID,myFoundry->foundryInfo->loading_capacity,myFoundry->foundryInfo->waiting_iron,myFoundry->foundryInfo->waiting_coal,myFoundry->foundryInfo->total_produce);
				WriteOutput(NULL,NULL,NULL,myFoundry->foundryInfo,FOUNDRY_STOPPED);
				//deleteWithIdFoundry(myFoundry->foundryInfo->ID);
				sem_destroy(&myFoundry->foundrySem);
				sem_destroy(&myFoundry->canProduce);		
				myFoundry->~foundry();
				pthread_exit(NULL);
	        }
		}
		//TIMOUTCASE
		WriteOutput(NULL,NULL,NULL,myFoundry->foundryInfo,FOUNDRY_STARTED);
		usleep(myFoundry->_interval - (myFoundry->_interval*0.01) + (rand()%(int)(myFoundry->_interval*0.02)));
		sem_wait(&myFoundry->foundrySem);
		myFoundry->foundryInfo->total_produce++;
		myFoundry->foundryInfo->waiting_iron--;
		myFoundry->foundryInfo->waiting_coal--;
		myFoundry->alreadyCounted--;
		FillFoundryInfo(myFoundry->foundryInfo,myFoundry->foundryInfo->ID,myFoundry->foundryInfo->loading_capacity,myFoundry->foundryInfo->waiting_iron,myFoundry->foundryInfo->waiting_coal,myFoundry->foundryInfo->total_produce);
		WriteOutput(NULL,NULL,NULL,myFoundry->foundryInfo,FOUNDRY_FINISHED);
		sem_post(&protectIron);
		sem_post(&protectCoal);
		sem_post(&myFoundry->foundrySem);
	}
}
//usleep(time-(time*0.01)+(rand()%(int)(time*0.02)));
void *transporter_thread(void *currTransporter){
	transporter *myTransporter; 
	myTransporter = (transporter* ) currTransporter;
	miner *currMiner;
	int currIndex;
	smelter *currSmelter;
	foundry *currFoundry;
	bool hasFoundPrior  = false;
	bool flag = false;
	WriteOutput(NULL,myTransporter->transporterInfo, NULL, NULL, TRANSPORTER_CREATED);
	sem_wait(&generalTransporterSem);
	OreType *newType = new OreType;
	//	cout<<"-------------------"<<endl;
	while(1){
		sem_post(&generalTransporterSem);
		sem_wait(&anyTranspoterSearching);
		if(total>0){
			
				int count =0 ;
				int currIndex = lastIndex+1;
				while( minersVec.size() >0 and count<=minersVec.size()){
					if(currIndex >= minersVec.size()){
						currIndex = 0; 
					}
					sem_wait(&minersVec[currIndex]->minerSem);
					if(minersVec[currIndex]->minerInfo->current_count>0){
							currMiner = minersVec[currIndex];
							currMiner->isAvailable = false;
							lastIndex = currIndex;
							flag = true;
			
							currMiner->minerInfo->current_count--;
							sem_post(&currMiner->minerSem);
							sem_wait(&protectTotal);
							sem_wait(&generalTransporterSem);
							total--;
							sem_post(&protectTotal);
							WriteOutput(currMiner->minerInfo, myTransporter->transporterInfo, NULL, NULL,TRANSPORTER_TRAVEL);
							usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
							break;
						}
						sem_post(&minersVec[currIndex]->minerSem);
						
					count++;
					currIndex++;
				}
				sem_post(&anyTranspoterSearching);
				if(flag){
					
					if(currMiner->minerInfo->current_count == 0 and currMiner->maximumProduceLimit == 0){
						sem_wait(&anyTranspoterSearching);
						*newType = currMiner->minerInfo->oreType;
						usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
						
						FillMinerInfo(currMiner->minerInfo, currMiner->minerInfo->ID,currMiner->minerInfo->oreType,currMiner->minerInfo->capacity,currMiner->minerInfo->current_count);
						FillTransporterInfo(myTransporter->transporterInfo,myTransporter->transporterInfo->ID ,newType);		
						WriteOutput(currMiner->minerInfo,myTransporter->transporterInfo,NULL,NULL,TRANSPORTER_TAKE_ORE);
						usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
						
						deleteWithId(currMiner->minerInfo->ID);
						sem_post(&anyTranspoterSearching);
						
					}
					else{
						currMiner->isAvailable = true;
						*newType = currMiner->minerInfo->oreType;
						usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
						
						FillMinerInfo(currMiner->minerInfo, currMiner->minerInfo->ID,currMiner->minerInfo->oreType,currMiner->minerInfo->capacity,currMiner->minerInfo->current_count);
						FillTransporterInfo(myTransporter->transporterInfo,myTransporter->transporterInfo->ID ,newType);
						WriteOutput(currMiner->minerInfo,myTransporter->transporterInfo,NULL,NULL,TRANSPORTER_TAKE_ORE);
						usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
						sem_post(&currMiner->canProduce);	
						//sleep		
					}
				flag = false;
				if(*myTransporter->transporterInfo->carry == IRON){
					//sem_wait(&protectIron);
					//cout<<"HERE"<<endl;
					if(smeltersVec.size()>0 or foundriesVec.size()){
						sem_wait(&protectIron);
						for(int i = 0; i<smeltersVec.size(); i++) {
						//cout<<"------264------"<<endl;
							sem_wait(&smeltersVec[i]->smelterSem);
							if(smeltersVec[i] != NULL and smeltersVec[i]->smelterInfo->oreType == IRON and smeltersVec[i]->smelterInfo->waiting_ore_count -smeltersVec[i]->alreadyCounted>0 and smeltersVec[i]->smelterInfo->waiting_ore_count < smeltersVec[i]->smelterInfo->loading_capacity ){
								//cout<<"*********************************267-------"<<endl;
								currSmelter = smeltersVec[i];
								WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_TRAVEL);
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
								hasFoundPrior = true;
								currSmelter->smelterInfo->waiting_ore_count++;
								//cout<<"HAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa"<<endl;
								smeltersVec[i]->alreadyCounted +=2;
								FillSmelterInfo(currSmelter->smelterInfo,currSmelter->smelterInfo->ID,currSmelter->smelterInfo->oreType,currSmelter->smelterInfo->loading_capacity,currSmelter->smelterInfo->waiting_ore_count,currSmelter->smelterInfo->total_produce);
								WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_DROP_ORE);
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
								sem_post(&currSmelter->canProduce);
								sem_post(&currSmelter->smelterSem);
								break;
							}
							sem_post(&smeltersVec[i]->smelterSem);
						}
						
						if(!hasFoundPrior){

							for(int i = 0; i<foundriesVec.size(); i++) {
								sem_wait(&foundriesVec[i]->foundrySem);
								if(foundriesVec[i] != NULL and foundriesVec[i]->foundryInfo->waiting_coal -foundriesVec[i]-> alreadyCounted > 0 and foundriesVec[i]->foundryInfo->waiting_iron < foundriesVec[i]->foundryInfo->loading_capacity){
							//cout<<"------300------"<<endl;
									currFoundry = foundriesVec[i];
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_TRAVEL);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	

									hasFoundPrior = true;
									currFoundry -> foundryInfo->waiting_iron ++;
									currFoundry-> alreadyCounted ++;

									FillFoundryInfo(currFoundry->foundryInfo,currFoundry->foundryInfo->ID,currFoundry->foundryInfo->loading_capacity,currFoundry->foundryInfo->waiting_iron,currFoundry->foundryInfo->waiting_coal,currFoundry->foundryInfo->total_produce);
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_DROP_ORE);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));

									sem_post(&currFoundry -> canProduce);
									sem_post(&foundriesVec[i]->foundrySem);
									break;
								}
								sem_post(&foundriesVec[i]->foundrySem);

							}
						}
						if(!hasFoundPrior){		
				
							for(int i = 0; i<smeltersVec.size(); i++) {
						//cout<<"------300------"<<endl;

							
								sem_wait(&smeltersVec[i]->smelterSem);
								if(smeltersVec[i] != NULL and smeltersVec[i]->smelterInfo->oreType == IRON and smeltersVec[i]->smelterInfo->waiting_ore_count < smeltersVec[i]->smelterInfo->loading_capacity ){
									//cout<<"------301-----"<<endl;
									currSmelter = smeltersVec[i];
									WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_TRAVEL);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									currSmelter->smelterInfo->waiting_ore_count++;			
									FillSmelterInfo(currSmelter->smelterInfo,currSmelter->smelterInfo->ID,currSmelter->smelterInfo->oreType,currSmelter->smelterInfo->loading_capacity,currSmelter->smelterInfo->waiting_ore_count,currSmelter->smelterInfo->total_produce);
									WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_DROP_ORE);	
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									sem_post(&smeltersVec[i]->smelterSem);
									//cout<<"------318-----"<<endl;
									
									hasFoundPrior = true;

									break;
								}
								sem_post(&smeltersVec[i]->smelterSem);
							}			
						}
						if(!hasFoundPrior){
							for(int i = 0; i<foundriesVec.size(); i++) {
								sem_wait(&foundriesVec[i]->foundrySem);
								if(foundriesVec[i] != NULL and foundriesVec[i]->foundryInfo->waiting_iron < foundriesVec[i]->foundryInfo->loading_capacity ){
						//	cout<<"------***********************------"<<endl;
									currFoundry = foundriesVec[i];
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_TRAVEL);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									
									hasFoundPrior = true;
									currFoundry -> foundryInfo->waiting_iron ++;
									FillFoundryInfo(currFoundry->foundryInfo,currFoundry->foundryInfo->ID,currFoundry->foundryInfo->loading_capacity,currFoundry->foundryInfo->waiting_iron,currFoundry->foundryInfo->waiting_coal,currFoundry->foundryInfo->total_produce);
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_DROP_ORE);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));
									
									sem_post(&foundriesVec[i]->foundrySem);
									break;
								}
								sem_post(&foundriesVec[i]->foundrySem);

							}
						}
						hasFoundPrior = false;
							
						
					}
					else{
						break;
					}
						
				}
				else if(*myTransporter->transporterInfo->carry == COPPER){
					if(smeltersVec.size()>0 ){
						sem_wait(&protectCopper);
						for(int i = 0; i<smeltersVec.size(); i++) {
						//cout<<"------264------"<<endl;
							sem_wait(&smeltersVec[i]->smelterSem);
							if(smeltersVec[i] != NULL and smeltersVec[i]->smelterInfo->oreType == COPPER and smeltersVec[i]->smelterInfo->waiting_ore_count -smeltersVec[i]->alreadyCounted>0 and smeltersVec[i]->smelterInfo->waiting_ore_count < smeltersVec[i]->smelterInfo->loading_capacity ){
								//cout<<"*********************************267-------"<<endl;
								currSmelter = smeltersVec[i];
								WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_TRAVEL);
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
								hasFoundPrior = true;
								currSmelter->smelterInfo->waiting_ore_count++;
								smeltersVec[i]->alreadyCounted +=2;
								
								FillSmelterInfo(currSmelter->smelterInfo,currSmelter->smelterInfo->ID,currSmelter->smelterInfo->oreType,currSmelter->smelterInfo->loading_capacity,currSmelter->smelterInfo->waiting_ore_count,currSmelter->smelterInfo->total_produce);
								WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_DROP_ORE);
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
								sem_post(&currSmelter->canProduce);
								sem_post(&currSmelter->smelterSem);
								break;
							}
							sem_post(&smeltersVec[i]->smelterSem);
						}
						if(!hasFoundPrior){		
					
								for(int i = 0; i<smeltersVec.size(); i++) {
							//cout<<"------300------"<<endl;

							
								sem_wait(&smeltersVec[i]->smelterSem);
								if(smeltersVec[i] != NULL and smeltersVec[i]->smelterInfo->oreType == COPPER and smeltersVec[i]->smelterInfo->waiting_ore_count < smeltersVec[i]->smelterInfo->loading_capacity){
									//cout<<"------301-----"<<endl;
									currSmelter = smeltersVec[i];
									WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_TRAVEL);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									currSmelter->smelterInfo->waiting_ore_count++;
									FillSmelterInfo(currSmelter->smelterInfo,currSmelter->smelterInfo->ID,currSmelter->smelterInfo->oreType,currSmelter->smelterInfo->loading_capacity,currSmelter->smelterInfo->waiting_ore_count,currSmelter->smelterInfo->total_produce);
									WriteOutput(NULL,myTransporter->transporterInfo,currSmelter->smelterInfo,NULL,TRANSPORTER_DROP_ORE);	
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									sem_post(&smeltersVec[i]->smelterSem);
									//cout<<"------318-----"<<endl;

									break;
								}
								sem_post(&smeltersVec[i]->smelterSem);
							}			
						}
						hasFoundPrior = false;
					}
					
				}
				else if(*myTransporter->transporterInfo->carry == COAL){
					sem_wait(&protectCoal);				
					for(int i = 0; i<foundriesVec.size(); i++) {
								sem_wait(&foundriesVec[i]->foundrySem);
								if(foundriesVec[i] != NULL and foundriesVec[i]->foundryInfo->waiting_iron -foundriesVec[i]-> alreadyCounted > 0  and foundriesVec[i]->foundryInfo->waiting_coal < foundriesVec[i]->foundryInfo->loading_capacity){
							//cout<<"------300------"<<endl;
									currFoundry = foundriesVec[i];
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_TRAVEL);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									hasFoundPrior = true;
									currFoundry -> foundryInfo->waiting_coal++;
									currFoundry-> alreadyCounted ++;

									FillFoundryInfo(currFoundry->foundryInfo,currFoundry->foundryInfo->ID,currFoundry->foundryInfo->loading_capacity,currFoundry->foundryInfo->waiting_iron,currFoundry->foundryInfo->waiting_coal,currFoundry->foundryInfo->total_produce);
									WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_DROP_ORE);
									usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
									
									sem_post(&currFoundry -> canProduce);
									sem_post(&foundriesVec[i]->foundrySem);
									break;
								}
								sem_post(&foundriesVec[i]->foundrySem);

							}
					if(!hasFoundPrior){
						for(int i = 0; i<foundriesVec.size(); i++) {
							sem_wait(&foundriesVec[i]->foundrySem);
							if(foundriesVec[i] != NULL and foundriesVec[i]->foundryInfo->waiting_coal < foundriesVec[i]->foundryInfo->loading_capacity ){
								currFoundry = foundriesVec[i];
								WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_TRAVEL);
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));			
								hasFoundPrior = true;
								currFoundry -> foundryInfo->waiting_coal++;
								FillFoundryInfo(currFoundry->foundryInfo,currFoundry->foundryInfo->ID,currFoundry->foundryInfo->loading_capacity,currFoundry->foundryInfo->waiting_iron,currFoundry->foundryInfo->waiting_coal,currFoundry->foundryInfo->total_produce);
								WriteOutput(NULL,myTransporter->transporterInfo,NULL,currFoundry->foundryInfo,TRANSPORTER_DROP_ORE);
								
								usleep(myTransporter->_interval - (myTransporter->_interval*0.01) + (rand()%(int)(myTransporter->_interval*0.02)));	
								
								sem_post(&foundriesVec[i]->foundrySem);
								break;
							}
							sem_post(&foundriesVec[i]->foundrySem);

						}
					}
					hasFoundPrior = false;
				}
			}
		}
		else if(minersVec.size() == 0 and total == 0){
			sem_post(&anyTranspoterSearching);
			break;
		}
		else {

			sem_post(&anyTranspoterSearching);
			sem_wait(&generalTransporterSem);
			sem_post(&generalTransporterSem);
		}
		
	}
	FillTransporterInfo(myTransporter->transporterInfo,myTransporter->transporterInfo->ID ,&(currMiner->minerInfo->oreType));
	WriteOutput(NULL,myTransporter->transporterInfo,NULL,NULL,TRANSPORTER_STOPPED);	
	myTransporter->~transporter();
	delete newType;
	pthread_exit(NULL);
}

int main(int argc, const char* argv[])
{	
	//assume that you took the input
	
	InitWriteOutput();
	int currIndex;
	int num_of_miners;
	int num_of_transporters;
	int num_of_smelters;
	int num_of_foundries;
	unsigned int interval;
	unsigned int capacity;
	unsigned int type;
	unsigned int max;

	sem_init(&generalTransporterSem,1,0);
	sem_init(&anyTranspoterSearching,1,1);
	sem_init(&protectTotal,1,1);
	sem_init(&protectIron,1,0);
	sem_init(&protectCopper,1,0);
	sem_init(&protectCoal,1,0);
	sem_init(&eraseFoundry,1,1);
	sem_init(&eraseSmelter,1,1);
	countOfEreasedMiner = 0;
	lastIndex = -1;
	total = 0;

	//-------------------------- GET INPUTS-----------------------------------------------
	cin>>num_of_miners;
	//cout<<"MINER "<<num_of_miners<<endl;
	pthread_t minersThreadArray[num_of_miners];
	for(int i =0;i<num_of_miners;i++){
		cin>>interval;
		cin>>capacity;
		cin>>type;
		cin>>max;
		if(type == 0){
			miner *newMiner = new miner(i,capacity,max, IRON, interval);
			minersVec.push_back(newMiner);
			pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
		} else if (type == 1) {
			miner *newMiner = new miner(i,capacity,max, COPPER, interval);
			minersVec.push_back(newMiner);
			pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
		} else {
			miner *newMiner = new miner(i,capacity,max, COAL, interval);
			minersVec.push_back(newMiner);
			pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
		}
		//cout<<"MINER "<<interval<<" "<<capacity<<" "<<type<<" "<<max<<endl; 
	}
	cin>>num_of_transporters;
	//cout<<"TRANSPORTER "<<num_of_transporters<<endl;
	pthread_t transportersThreadArray[num_of_transporters];
	for(int i =0;i<num_of_transporters;i++){
		cin>>interval;
		transporter *newTrasporter = new transporter(i,NULL,interval);
		pthread_create(&transportersThreadArray[i],NULL,transporter_thread,(void *)newTrasporter); 
		//cout<<"TRANSPORTER "<<interval<<endl; 


	}

	cin>>num_of_smelters;
	pthread_t smeltersThreadArray[num_of_smelters];
	for (int i =0; i< num_of_smelters;i++){
		cin>>interval;
		cin>>capacity;
		cin>>type;
		if(type == 0){
			smelter *newSmelter = new smelter(i,IRON,capacity,interval);
			smeltersVec.push_back(newSmelter);
			pthread_create(&smeltersThreadArray[i],NULL,&smelter_thread,(void*)newSmelter); 
		} else if (type == 1) {
			smelter *newSmelter = new smelter(i,COPPER,capacity,interval);
			smeltersVec.push_back(newSmelter);
			pthread_create(&smeltersThreadArray[i],NULL,&smelter_thread,(void*)newSmelter); 
		}
		//cout<<"SMELTER "<<interval<<" "<<capacity<<" "<<type<<endl; 

		
	}
	cin>>num_of_foundries;

	pthread_t foundriesThreadArray[num_of_foundries];
	for (int i =0; i< num_of_foundries;i++){
		cin>>interval;
		cin>>capacity;
		foundry *newFoundry = new foundry(i,capacity,interval);
		foundriesVec.push_back(newFoundry);
		pthread_create(&foundriesThreadArray [i],NULL,&foundry_thread,(void*)newFoundry); 
		//cout<<"FOUNDRY "<<interval<<" "<<capacity<<endl; 

	}

	//ALLAH MUHAMMED AŞKINA INPUT AL 


	//protect iron , cole, copper semaphoreları initilize edilecek
	//----------------------------------Creating threads--------------------------------------
	/*pthread_t minersThreadArray[num_of_miners];
	pthread_t transportersThreadArray[num_of_transporters];
		pthread_t smeltersThreadArray[num_of_smelters];
		pthread_t foundriesThreadArray[num_of_foundries];
	for (int i =0; i< num_of_miners;i++){
		miner *newMiner = new miner(i,2,4, IRON,100);
		minersVec.push_back(newMiner);
		pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
	}
	for (int i =2; i< num_of_miners+2;i++){
		miner *newMiner = new miner(i,2,4, COAL,100);
		minersVec.push_back(newMiner);
		pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
	}
	for (int i =4; i< num_of_miners+4;i++){
		miner *newMiner = new miner(i,2,8, COPPER,100);
		minersVec.push_back(newMiner);
		pthread_create(&minersThreadArray[i],NULL,miner_thread, (void*)newMiner);
	}
	for (int i =0; i< num_of_transporters;i++){
		transporter *newTrasporter = new transporter(i,NULL,100);
		pthread_create(&transportersThreadArray[i],NULL,transporter_thread,(void *)newTrasporter); 
	}
	for (int i =0; i< num_of_smelters;i++){
		smelter *newSmelter = new smelter(i,COPPER,2,100);
		smeltersVec.push_back(newSmelter);
		pthread_create(&smeltersThreadArray[i],NULL,&smelter_thread,(void*)newSmelter); 
	}
	for (int i =0; i< num_of_foundries;i++){
		foundry *newFoundry = new foundry(i,2,100);
		foundriesVec.push_back(newFoundry);
		pthread_create(&foundriesThreadArray [i],NULL,&foundry_thread,(void*)newFoundry); 
	}*/
	for(int i =0;i<num_of_miners;i++){
		pthread_join(minersThreadArray[i],NULL);
	}
	for(int i =0;i<num_of_transporters;i++){
		pthread_join(transportersThreadArray[i],NULL);
	}
	for(int i =0;i<num_of_smelters;i++){
		pthread_join(smeltersThreadArray[i],NULL);
	}
	for(int i =0;i<num_of_foundries;i++){
		pthread_join(foundriesThreadArray[i],NULL);
	}

	sem_destroy(&generalTransporterSem);
	sem_destroy(&anyTranspoterSearching);
	sem_destroy(&protectTotal);
	sem_destroy(&protectIron);
	sem_destroy(&protectCopper);
	sem_destroy(&protectCoal);
	sem_destroy(&eraseFoundry);
	sem_destroy(&eraseSmelter);
	return 0;
}

