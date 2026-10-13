#define JSON_TRAINING_FILE_NAME "/TrainingData.json"



bool GetOccupancyStateOfRoom(const char* roomName) ;

typedef struct BathRoomMetrics
{
	int total_no_of_day_visits = 0 ;     /* 7 AM to 8PM  */
  int day_visit_idx ;
	int total_no_of_night_visits = 0 ;   /* 8 PM to 7 AM */
  int night_visit_idx ;

	int  duration_of_day_visits[50];
	bool duration_of_day_visit_checked_for_anomaly[50] ;

  int  duration_of_night_visits[50];
  bool duration_of_night_visit_checked_for_anomaly[50] ;

  time_t timeOfDayVisit[50] ;
  time_t timeOfNightVisit[50] ; 
  bool newDataArrived = false ; 
  char alertMessage[100] ;

  /* This flag will be used by anomaly checker to a new data has 
     arrived or not. If new data has arrived Anomaly checker
     will not keep processing the existing data and create 
     duplicate alerts. The AnomalyChecking Routine main job
     at periodic interval is is  check this flag. If the flag
     is changed to true, it will run CUSUM and Z_score check and see an anomaly
     is there is not, then set the flag to false. 
    */
     
	 
	 /* following are variable for calculating the trend
	    using CU-SUM Cumiltive Sum Method
	  */
	  
	float S_no_of_day_visits = 0.0 ;
	float k_no_of_day_visits ;
	float h_no_of_day_visits ;
	 
	 	 
	float S_no_of_night_visits = 0.0;
	float k_no_of_night_visits ;
	float h_no_of_night_visits ;
	 
	 	
	bool trend_flag_day_vists = false ;
	bool trend_flag_night_vists = false ;
	

	bool spike_flag_for_no_day_vists = false  ; 
	bool spike_flag_for_no_night_vists = false ;
	bool spike_flag_for_duration_day_vists = false ;
	bool spike_flag_for_duration_night_vists = false;
	
	int alertCount =  0; 
	 
} bathRoomMetrics_t;

bathRoomMetrics_t BathRoomData ;


typedef struct TrainingData
{
	int mean_no_of_day_visits ;
	int mean_no_of_night_visits ; 

	int median_no_of_day_visits;
	int median_no_of_night_visits ;
	
	int MAD_no_of_day_visits;      /* (median absolute deviation) */
	int MAD_no_of_night_visits ;   /* (median absolute deviation) */
	
	int mean_duration_of_day_visits ;
	int mean_duration_of_night_visits ;
	
	int median_duration_of_day_visits ;
	int median_duration_of_night_visits ;

	int MAD_duration_of_day_visits ;   /* (median absolute deviation) */
	int MAD_duration_of_night_visits ; /* (median absolute deviation) */

  int median_no_of_bedRoomMovements ;
  int MAD_no_of_bedRoomMovements;


  unsigned long BEDROOM_MAX_DURATION ;
  unsigned long BEDROOM_MOVEMENT_SCORE_LIMIT ; 
  unsigned long MAX_LIVINGROOM_DURATION_NIGHT ;
  unsigned long MAX_KITCHEN_DURATION_NIGHT ; 
  unsigned long MAX_BATHROOM_DURATION ;
  unsigned long BATHROOM_ALERT_THRESHOLD ;
  unsigned long DAY_TIME_BATHROOM_VISIT_BASELINE ;
	
} trainingData_t;

trainingData_t  TrainedData ;

/* *****************************************************************************
Function Name : LoadTrainingData
INPUT PARAMETERS :
    JSON FILE WITH TRAINED DATA VALUES
RETURN
   None
FUNCTION
   Read the JSON file and load the trained values/weights into data structure  struct TrainingData
Author : Girish Kumar
******************************************************************************/

void LoadTrainingData()
{
	  File jsonFile ;
     
    JsonDocument  doc;

    DEBUG_PRINTF("LoadBathRoomTrainingData starting\n") ;
    jsonFile = SD.open(JSON_TRAINING_FILE_NAME, FILE_READ);
  
    if (!jsonFile)
    {
        DEBUG_PRINTF("Unable to open %s\n",JSON_TRAINING_FILE_NAME);
       return ;
    }
    deserializeJson(doc, jsonFile);
	
	  const char* mean_no_of_day_visitsCharPtr   = doc["mean_no_of_day_visits"] ; // "xxxxxxxxxxxxxxxxxxxx"
    
    const char* mean_no_of_night_visitsCharPtr = doc["mean_no_of_night_visits"] ; // "xxxxxxxxxxxxxxxxxxxx"
       
    const char* median_no_of_day_visitsCharPtr = doc["median_no_of_day_visits"] ; // "xxxxxxxxxxxxxxxxxxxx"
  
    const char* median_no_of_night_visitsCharPtr = doc["median_no_of_night_visits"] ; // "xxxxxxxxxxxxxxxxxxxx"

    const char* MAD_no_of_day_visitsCharPtr    = doc["MAD_no_of_day_visits"] ; // "xxxxxxxxxxxxxxxxxxxx"
  
    const char* MAD_no_of_night_visitsCharPtr  = doc["MAD_no_of_night_visits"] ; // "xxxxxxxxxxxxxxx"  

    const char* mean_duration_of_day_visitsCharPtr = doc["mean_duration_of_day_visits "] ; // "xxxxxxxxxxxxxx"
  
    const char* mean_duration_of_night_visitsCharPtr = doc["mean_duration_of_night_visits"] ; // "xxxxxxxxxxxxxx"

    const char* median_duration_of_day_visitsCharPtr = doc["median_duration_of_day_visits"] ;
  
    const char* median_duration_of_night_visitsCharPtr = doc["median_duration_of_night_visits"] ;

	  const char* MAD_duration_of_day_visitsCharPtr = doc["MAD_duration_of_day_visits"] ;
	
    const char* MAD_duration_of_night_visitsCharPtr = doc["MAD_duration_of_night_visits"];

    /* Parameters for BedRoom */
    const char* median_no_of_bedRoomMovementsCharPtr =  doc["median_no_of_bedRoomMovements"] ;
  
    const char* MAD_no_of_bedRoomMovementsCharPtr =    doc["MAD_no_of_bedRoomMovements"];
   /* End Parameters for BedRoom */
    

    const char* BEDROOM_MAX_DURATIONCharPtr = doc["BEDROOM_MAX_DURATION"];
  
    const char* BEDROOM_MOVEMENT_SCORE_LIMITCharPtr = doc["BEDROOM_MOVEMENT_SCORE_LIMIT"];  
  
    const char* MAX_LIVINGROOM_DURATION_NIGHTCharPtr = doc["MAX_LIVINGROOM_DURATION_NIGHT"]; 
  
    const char* MAX_KITCHEN_DURATION_NIGHTCharPtr = doc["MAX_KITCHEN_DURATION_NIGHT"];
  
    const char* MAX_BATHROOM_DURATIONCharPtr = doc["MAX_BATHROOM_DURATION"];
  
    const char* BATHROOM_ALERT_THRESHOLDCharPtr =   doc["BATHROOM_ALERT_THRESHOLD"];
  
    const char* DAY_TIME_BATHROOM_VISIT_BASELINECharPtr = doc["DAY_TIME_BATHROOM_VISIT_BASELINE"]; 
	
	  TrainedData.mean_no_of_day_visits = String(mean_no_of_day_visitsCharPtr).toInt();
      
    TrainedData.mean_no_of_night_visits = String(mean_no_of_night_visitsCharPtr).toInt();
	
	  TrainedData.median_no_of_day_visits =  String(median_no_of_day_visitsCharPtr).toInt();
	  
    TrainedData.median_no_of_night_visits = String(median_no_of_night_visitsCharPtr).toInt();
	
	  TrainedData.MAD_no_of_day_visits =   String(MAD_no_of_day_visitsCharPtr).toInt();
	  
    TrainedData.MAD_no_of_night_visits = String(MAD_no_of_night_visitsCharPtr).toInt();
	
    /* Parameters for BedRoom */
    
	  TrainedData.median_no_of_bedRoomMovements =   String(median_no_of_bedRoomMovementsCharPtr).toInt() ;
	  
    TrainedData.MAD_no_of_bedRoomMovements = String(MAD_no_of_bedRoomMovementsCharPtr).toInt() ;

	  
    /*  Parameters for BedRoom */
 
	  TrainedData.median_duration_of_day_visits = String(median_duration_of_day_visitsCharPtr).toInt();
	  
    TrainedData.median_duration_of_night_visits = String(median_duration_of_night_visitsCharPtr).toInt();
	
	  TrainedData.MAD_duration_of_day_visits  = String(MAD_duration_of_day_visitsCharPtr).toInt();
	  
    TrainedData.MAD_duration_of_night_visits  = String(MAD_duration_of_night_visitsCharPtr).toInt();
	
	  BathRoomData.k_no_of_day_visits = 0.5 * TrainedData.MAD_no_of_day_visits ;
    
    BathRoomData.h_no_of_day_visits = 5 * TrainedData.MAD_no_of_day_visits ;
	
	  BathRoomData.k_no_of_night_visits = 0.5 * TrainedData.MAD_no_of_night_visits ;
    
    BathRoomData.h_no_of_night_visits = 5 * TrainedData.MAD_no_of_night_visits ;

    TrainedData.BEDROOM_MAX_DURATION = String(BEDROOM_MAX_DURATIONCharPtr).toInt();

    TrainedData.BEDROOM_MOVEMENT_SCORE_LIMIT  = String(BEDROOM_MOVEMENT_SCORE_LIMITCharPtr).toInt(); 
    
    TrainedData.MAX_LIVINGROOM_DURATION_NIGHT  = String(MAX_LIVINGROOM_DURATION_NIGHTCharPtr).toInt();
    
    TrainedData.MAX_KITCHEN_DURATION_NIGHT = String(MAX_KITCHEN_DURATION_NIGHTCharPtr).toInt();
    
    TrainedData.MAX_BATHROOM_DURATION  = String(MAX_BATHROOM_DURATIONCharPtr).toInt();
    
    TrainedData.BATHROOM_ALERT_THRESHOLD = String(BATHROOM_ALERT_THRESHOLDCharPtr).toInt();
    
    TrainedData.DAY_TIME_BATHROOM_VISIT_BASELINE  = String(DAY_TIME_BATHROOM_VISIT_BASELINECharPtr).toInt();

    jsonFile.close();
}


void RecordtimeOfBathroomDayVisit(time_t currentTime)
{
  int idx ;
  idx =  BathRoomData.day_visit_idx ;
  BathRoomData.timeOfDayVisit[idx] = currentTime ;
}

time_t  GetTimeOfBathroomDayVisit()
{
  int idx ;
  idx =  BathRoomData.day_visit_idx ;
  return BathRoomData.timeOfDayVisit[idx]  ;
}

void RecordtimeOfBathroomNightVisit(time_t currentTime)
{
  int idx ;
  idx =  BathRoomData.night_visit_idx ; 
  BathRoomData.timeOfNightVisit[idx] = currentTime ;
}

time_t GetTimeOfBathroomNightVisit()
{
  int idx ;
  idx =  BathRoomData.night_visit_idx  ; 
  return BathRoomData.timeOfNightVisit[idx] ;
}

void SetNewDataArrivedFlag()
{
  /* 
     This routine will be executed by onDataReceived routine when an MQTT message
     indicating Presence has detected in Bathroom.
  */

  BathRoomData.newDataArrived  = true ;
}

void ReSetNewDataArrivedFlag()
{
  /* This routine will be executed by anomaly checking routine after running
     the period anomaly check
  */
  BathRoomData.newDataArrived  = false ;
}
void IncrementBathRoomDayVisits()
{
  DEBUG_PRINTF("IncrementBathRoomDayVisits\n") ;
	BathRoomData.total_no_of_day_visits++; 
  if (BathRoomData.day_visit_idx > 48)
  {
       InitBathRoomUsageData();
       BathRoomData.day_visit_idx = 1;
  }
  else
  {
       BathRoomData.day_visit_idx++ ;
  }
}

void IncrementBathRoomNightVisits()
{
  DEBUG_PRINTF("IncrementBathRoomNightVisits\n") ;
	BathRoomData.total_no_of_night_visits++ ;
  if (BathRoomData.night_visit_idx > 48) 
  {   
      InitBathRoomUsageData();
      BathRoomData.night_visit_idx = 1 ;
  }
  else
  {
      BathRoomData.night_visit_idx++ ;
  }
}

void DecrementBathRoomDayVisits()
{
    /* 
       This routine is executed when  spike from the sensor or false signal for sensor
       arrives. This is detected when the room becomes empty in a short span.
        
     */

    DEBUG_PRINTF("DecrementBathRoomDayVisits\n") ;
    if (BathRoomData.total_no_of_day_visits != 0)
     {
        BathRoomData.day_visit_idx-- ;
        BathRoomData.total_no_of_day_visits--; 
     }
}
void DecrementBathRoomNightVisits()
{

    /* 
       This routine is executed when  spike from the sensor or false signal for sensor
       arrives. 
     */
    DEBUG_PRINTF("DecrementBathRoomNightVisits\n") ;
    if (BathRoomData.total_no_of_night_visits > 1)
    {
	      BathRoomData.total_no_of_night_visits-- ;
        BathRoomData.night_visit_idx-- ;
    }
}
void UpdateBathRoomDayVisitDuration(int duration)
{
	BathRoomData.duration_of_day_visits[BathRoomData.day_visit_idx] = duration ;
  BathRoomData.duration_of_day_visit_checked_for_anomaly[BathRoomData.day_visit_idx] = false ;
}
void UpdateBathRoomNightVisitDuration(int duration)
{
	BathRoomData.duration_of_night_visits[BathRoomData.night_visit_idx] = duration ;
  BathRoomData.duration_of_day_visit_checked_for_anomaly[BathRoomData.day_visit_idx] = false ; 
}







void InitBathRoomUsageData()
{
  /*
       As the system is running constantly, the observation of bathroom usage need to reset daily
       So that it does not keet accumulating. The function implements the logic to reset
       bathroom data.
      
    */
    
    int idx ;

    BathRoomData.total_no_of_day_visits = 0 ;  
	  BathRoomData.total_no_of_night_visits = 0;  
    
    for (idx=0;idx<50;idx++)
    { 
	     BathRoomData.duration_of_day_visits[idx] = 0;
	     BathRoomData.duration_of_night_visits[idx] = 0;
       BathRoomData.duration_of_day_visit_checked_for_anomaly[idx] = false ;
       BathRoomData.duration_of_night_visit_checked_for_anomaly[idx] = false ;
    }
 

  return ;
}

void PrintBathRoomUsageData()
{
   int idx ;
   DEBUG_PRINTF("-------------------------------Bath Room Data -------------------------\n") ;
   DEBUG_PRINTF("total_no_of_day_visits %d\n",BathRoomData.total_no_of_day_visits) ;
   DEBUG_PRINTF("total_no_of_night_visits  %d\n",BathRoomData.total_no_of_night_visits) ;
   DEBUG_PRINT("duration_of_day_visits  ") ;
   
     
   for (idx = 0 ; idx< 50; idx++)
   {
      DEBUG_PRINT(BathRoomData.duration_of_day_visits[idx]) ;
      DEBUG_PRINT(", ") ;    
   }
   DEBUG_PRINTF("\n") ;
   DEBUG_PRINT("duration_of_night_visits  ") ;
     
   for (idx = 0 ; idx< 50; idx++)
   {
      DEBUG_PRINT(BathRoomData.duration_of_night_visits[idx]) ; 
      DEBUG_PRINT(", ") ;      
   }
   DEBUG_PRINTF("\n") ;
DEBUG_PRINTF("---------------------------------End of Data------------------------------\n") ;
}

void RunAnomalyCheckForBathroomUsage()
{
    
  int i ;
  bool day_visit_count_anomaly ;
  bool night_visit_count_anomaly  ;
  bool day_visit_duration_anomaly ;
  bool night_visit_duration_anomaly ;
  struct tm ti ;

  DEBUG_PRINTF("------------------ start Anomaly Check ----------------------- \n") ;
  PrintBathRoomUsageData();
  if (BathRoomData.newDataArrived == false)
  {
      /* NO new data has arrived sp no need to anomaly check */
      DEBUG_PRINTF("Anomaly checker not executed as there is no new data");
      return ;
  }
  DEBUG_PRINTF("Anomaly checker running on Bathroom data\n");
  if (BathRoomData.total_no_of_day_visits >3)
  {
      day_visit_count_anomaly = CheckForAnomaly((float) BathRoomData.total_no_of_day_visits, "day_visit_count") ;
  }

  if (BathRoomData.total_no_of_night_visits > 3)
  {
      night_visit_count_anomaly = CheckForAnomaly((float) BathRoomData.total_no_of_night_visits, "night_visit_count") ;
  }

  if (BathRoomData.total_no_of_day_visits > 0)
  {
       i = BathRoomData.day_visit_idx  ;
  }
  else
  {
        i = 0 ;
  }
  if ( BathRoomData.duration_of_day_visits[i] >0)
  {
      if (BathRoomData.duration_of_day_visit_checked_for_anomaly[i] == false)
      {
         BathRoomData.duration_of_day_visit_checked_for_anomaly[i] = true; 
         day_visit_duration_anomaly = CheckForAnomaly((float) BathRoomData.duration_of_day_visits[i], "day_visit_duration") ;
      }
      
  }

  if (BathRoomData.total_no_of_night_visits >0)
  {
     i = BathRoomData.night_visit_idx  ;
  }
  else
  {
     i = 0 ;
  }

  if (BathRoomData.duration_of_night_visits[i]>0)
  {
      if (BathRoomData.duration_of_night_visit_checked_for_anomaly[i] == false)
      {
          BathRoomData.duration_of_night_visit_checked_for_anomaly[i] = true; 
          night_visit_duration_anomaly = CheckForAnomaly((float) BathRoomData.duration_of_night_visits[i], "night_visit_duration") ;
          DEBUG_PRINTF(" Checking for anomaly: BathRoomData.night_visit_idx = %d\n", BathRoomData.night_visit_idx) ;
      }
  }

/* --------------------------------------------------------------------------------------------------------- */

  if  (day_visit_count_anomaly)
  {
      ti = convertEpochToHumanReadableFormat(BathRoomData.timeOfDayVisit[BathRoomData.total_no_of_day_visits]) ;
 
      sprintf(BathRoomData.alertMessage, 
              "We detected an unusual number of bathroom visits \(%d\)  during day time, today at  %d:%d:%d. You may want to check. ",
               BathRoomData.total_no_of_day_visits,
                ti.tm_hour,        
                ti.tm_min,      
                ti.tm_sec);                

      publishAlert("Toilet-1", BathRoomData.alertMessage); 
      BathRoomData.total_no_of_day_visits = 0 ;
  }

  if (night_visit_count_anomaly)
  {
      ti = convertEpochToHumanReadableFormat(BathRoomData.timeOfNightVisit[BathRoomData.total_no_of_night_visits]) ;
      sprintf(BathRoomData.alertMessage, 
              "We had noticed  an unusual number of bathroom visits \(%d\) today during night at %d:%d:%d. You may want to check.",
               BathRoomData.total_no_of_night_visits,
                ti.tm_hour,        
                ti.tm_min,      
                ti.tm_sec);

      publishAlert("Toilet-1", BathRoomData.alertMessage);
      BathRoomData.total_no_of_night_visits = 0 ;
  }

  if (day_visit_duration_anomaly) 
  {
      i = BathRoomData.day_visit_idx ;
      ti = convertEpochToHumanReadableFormat(BathRoomData.timeOfDayVisit[i]) ;

      sprintf(BathRoomData.alertMessage, 
              "More time than usual time \(%d minutes\) was spent in the bathroom during day today at %d:%d:%d. You may want to check",
              BathRoomData.duration_of_day_visits[i],
              ti.tm_hour,        
              ti.tm_min,      
              ti.tm_sec);

      publishAlert("Toilet-1", BathRoomData.alertMessage);  
  }

  if (night_visit_duration_anomaly)
  {
      // Get the time of incident from epoc
      i = BathRoomData.night_visit_idx ; 
      ti = convertEpochToHumanReadableFormat(BathRoomData.timeOfNightVisit[i]) ;
      
      sprintf(BathRoomData.alertMessage, 
              "More time than usual time \(%d minutes\) was spent in the bathroom during night today at %d:%d:%d",
                BathRoomData.duration_of_night_visits[i],
                ti.tm_hour,        
                ti.tm_min,      
                ti.tm_sec);

      publishAlert("Toilet-1", BathRoomData.alertMessage);
  }
  PrintBathRoomUsageData() ;
  
  DEBUG_PRINTF("------------------ End Anomaly Check ----------------------- \n") ;
  //InitBathRoomUsageData();  ;
  ReSetNewDataArrivedFlag() ;	 
}


void CheckProlongedUseOfBathRoom()
{
      time_t diff ;

      struct tm ti ;

      time_t now = getEpochTime() ;
      
      if (GetOccupancyStateOfRoom("Toilet-1") == true)
      {
        /* Check the duration of occupancy */
         int occupiedTime ;
         occupiedTime =  GetOccupiedEpocTimeOfRoom("Toilet-1");

         diff = now - occupiedTime;
         
         ti = convertEpochToHumanReadableFormat(occupiedTime) ;
        

         if (diff > TrainedData.MAX_BATHROOM_DURATION) 
         {
            sprintf(BathRoomData.alertMessage, 
                    "An extended period of bathroom occupancy of \( %d \) has been detected at time  today %d:%d:%d today",
                       diff,
                       ti.tm_hour,        
                       ti.tm_min,      
                       ti.tm_sec);
            publishAlert("Toilet-1",BathRoomData.alertMessage);
         }

      }
}
