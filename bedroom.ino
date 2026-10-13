

typedef struct BedRoomMetrics
{

   int BedRoomMovementsCounter  ;
   	 	 
	 float S_no_of_bedRoomMovements ;
	 float K_no_of_bedRoomMovements ;
	 float H_no_of_bedRoomMovements ;

} BedRoomMetrics_t ;

struct BedRoomMetrics BedRoomData ;


bool GetOccupancyStateOfRoom(const char* roomName) ;


void InitBedRoomData()
{
    BedRoomData.BedRoomMovementsCounter = 0 ; 
}

void incrementBedRoomMovementCounter()
{
    BedRoomData.BedRoomMovementsCounter++ ;
}

void RunAnomalyCheckForBedRoom()
{
   // If too many movements are detected 
  bool movementsAnomaly  ;
 
  if (BedRoomData.BedRoomMovementsCounter >0) 
  {
      movementsAnomaly = CheckForAnomaly((float) BedRoomData.BedRoomMovementsCounter, "BedRoomMovements") ;
  }
  if (movementsAnomaly == true)
  {
        publishAlert("BedRoom", "Unusal Movements detected in BedRoom during night");
        /* 
           Reset the movement counter so that the alert don't get triggered  
           Increment the limit to avoid period alert , if needed 
        */
        BedRoomData.BedRoomMovementsCounter = 0 ; 
  }
        
}

void CheckProlongedUseOfBedRoom()
{
      time_t diff ;
      int idx = findRoomIndex("BedRoom");
      time_t now = getEpochTime() ;
      char message[50] ;
      

      if (GetOccupancyStateOfRoom("BedRoom") == true)
      {
        /* Check the duration of occupancy */
         diff = now - GetOccupiedEpocTimeOfRoom("BedRoom");

         if (diff > TrainedData.BEDROOM_MAX_DURATION) 
         {
              sprintf(message, "Prolonged Use of BedRoom detected %d mins", GetOccupiedEpocTimeOfRoom("BedRoom"));
              publishAlert("BedRoom",message );
         }
         
      }
}