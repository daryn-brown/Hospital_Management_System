/*
	Conglomerate Hospice Ltd, Patient Management System
	Developers: Shawn Grant       [2002432]
				Courtney Williams [2006427]
				Daryn Brown       [2002414]
				Tyreque Hibbert   [2005320]
				Nikayle Bennett   [1303941]
	Tutor     : Sachell Linch McInnis
	Occurence : UN3
	Date      : March 1, 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// global declarations here
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x)) //allows positioning the cursor (see menu)
// colors
#define RED "\x1b[31m"	// use red for error messages
#define GREEN "\x1b[32m"  // use green for success messages
#define YELLOW "\x1b[33m" // use yellow for warnings/rules
#define BLUE "\x1b[34m"
#define PURPLE "\x1b[35m"
#define CYAN "\x1b[36m" // used for headings
#define WHITE "\x1b[0m" // use as the default text color

// limits
#define MAX_PATIENTS 35
#define MAX_ROOMS 21
#define MAX_BEDS_PER_ROOM 5
#define MAX_PATIENTS_PER_ROOM 5

// structs
typedef struct {
	int id;
	char fname[25];
	char lname[25];
	char status[9];
	int birthYear;
	int age;
	char admissionStatus[9];
} Patient;

typedef struct {
	int roomNum;
	char respirator[10];
	int numBeds;
	char personnel[7];// "nurse" or "doctor" or "none".
	int numPatients; //number of patients in the room
	int patientIds[MAX_PATIENTS_PER_ROOM]; // array of patients assigned
} Room;

// function prototypes
void splashScreen();
void populateArraysFromFile(char[], char[], Patient[], Room[], int *, int *);
int menu();
void addPatient(Patient[], int *numPatients); // adds a patient to the array and increases the number of patients
void addRoom(Room[], int *numRooms); // adds a room to the array and increases the number of rooms
void changePatientStatus(Patient[], int); // change a patient's status
void autoAssignRooms(Patient[], Room[], int numPatients, int numRooms);// auto assign rooms
void dischargePatient(Patient[], int numPatients, Room rooms[], int numrooms);
void attendanceByAgeReport(Patient[], Room[], int numPatients, int numRooms);
void admittedPatientsListReport(Patient[], Room[], int numPatients, int numRooms);
void covid19StatusReport(Patient[], int numPatients);
int getRoomOfPatient (int, Room[], int);
void removePatientFromRoom (int, Room[], int);
void saveChanges(char[], char[], Patient[], Room[], int, int); // writes data to the two files
void exitProgram(); // says goodbye and terminates the program
void swap(Patient*, Patient*);//used in selection sort algorithm

// auxillary functions
void changeTextColor(char *color); // use with the colors defined above
void clearScreen(); // use this instead of system("cls")
void delay(int milliseconds);

// driver function
int main()
{
	Patient patients[MAX_PATIENTS]; // arrray to store all patients
	Room rooms[MAX_ROOMS];			// array to store info on all rooms
	char patientsfilename[100], roomsfilename[100];
	int numPatients = 0, numRooms = 0; // totals
	int choice;

	splashScreen();

	// after this call, the two arrays should have the data from the files in them
	// and the room and patient numbers should be updated
	populateArraysFromFile(
		patientsfilename,
		roomsfilename,
		patients,
		rooms,
		&numPatients,// pass by reference
		&numRooms); // pass by reference

	choice = menu();
	// will run until the user selects an exit option
	while (1) //1 is always true
	{
		switch (choice) 
		{
		case 1:
			addPatient(patients, &numPatients);
			break;

		case 2:
			addRoom(rooms, &numRooms);
			break;

		case 3:
			changePatientStatus(patients, numPatients);
			break;

		case 4:
			autoAssignRooms(patients, rooms, numPatients, numRooms);
			break;

		case 5:
      		dischargePatient(patients, numPatients, rooms, numRooms);
			break;

		case 6:
			attendanceByAgeReport(patients, rooms, numPatients, numRooms);
			break;

		case 7:
			admittedPatientsListReport(patients, rooms, numPatients, numRooms);
			break;

		case 8:
			covid19StatusReport(patients, numPatients);
			break;

		case 9: // exit and save
			saveChanges(
				patientsfilename,
				roomsfilename,
				patients,
				rooms,
				numPatients,
				numRooms);
			exitProgram();
			break;

		case 0: // without save
			exitProgram();
			break;

		default:
			changeTextColor(RED);
			printf("\n\nINVALID OPTION PLEASE RE-ENTER\n");
			delay(1000); // 1 second delay
		}

		choice = menu();
	}

	return 0;
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// Displays an introductory/startup page
void splashScreen() {
	clearScreen();
	changeTextColor(CYAN);

	printf(" _____                   _                                _             \n");
	printf("/ ____|                 | |                              | |            \n");
	printf("| |     ___  _ __   __ _| | ___  _ __ ___   ___ _ __ __ _| |_ ___        \n");
	printf("| |    / _ \\| '_ \\ / _` | |/ _ \\| '_ ` _ \\ / _ \\ '__/ _` | __/ _ \\ \n");
	printf("| |___| (_) | | | | (_| | | (_) | | | | | |  __/ | | (_| | ||  __/ \n");
	printf(" \\_____\\___/|_| |_|\\__, |_|\\___/|_| |_| |_|\\___|_|  \\__,_|\\__\\___|  \n");
	printf("                    __/ |                                          \n");
	printf("                   |___/                                           \n");

	printf("  _    _                 _            _     _      _         \n");
	printf(" | |  | |               (_)          | |   | |    | |        \n");
	printf(" | |__| | ___  ___ _ __  _  ___ ___  | |   | |_ __| |        \n");
	printf(" |  __  |/ _ \\/ __| '_ \\| |/ __/ _ \\ | |   | __/ _` |     \n");
	printf(" | |  | | (_) \\__ \\ |_) | | (_|  __/ | |___| || (_| |_     \n");
	printf(" |_|  |_|\\___/|___/ .__/|_|\\___\\___| |______\\__\\__,_(_) \n");
	printf("                  | |                                        \n");
	printf("                  |_|                                        \n");

	changeTextColor(WHITE); // reset to default
	printf("\tDeveloped By Peshengweng Studios (c) 2021\n");

	delay(1000);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// gets the data from the files and puts them into the appropriate struct
// patientList[] and roomList[] are passed from main so the new info is
// reflected in main
void populateArraysFromFile (char patientsFilename[], char roomsFilename[], 
							Patient patientList[], Room roomList[], int *numPatients,int *numRooms) 
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);//get current time & date
	FILE *patientFile, *roomFile;
	int i = 0, j = 0, tmp;
	char create, patientsIDS[40]; // store the patient IDs as one string until split into an array
	
	int year = tm.tm_year + 1900; //the current year

	clearScreen(); 

	// ask the user for the filename until it is a valid file
	do {
		printf("\nEnter the filepath of the " BLUE "patients file : " WHITE);
		scanf("%s", patientsFilename);

		patientFile = fopen(patientsFilename, "r"); // attempt access

		// feedback
		if (patientFile == NULL) 
    	{
			printf( YELLOW "\t'%s' does not exist. " WHITE "Create it? [Y/N]: ", patientsFilename);
			scanf(" %c", &create);

			// create the file
			if (create == 'Y' || create == 'y') {
				patientFile = fopen(patientsFilename, "w+");
				changeTextColor(GREEN);
				printf("\t%s Created\n", patientsFilename);
			}
		} 
		else 
		{
			changeTextColor(GREEN);
			printf("\tFile located\n");
		}

		changeTextColor(WHITE);
	} while (patientFile == NULL);

	// ask the user for the filename until it is a valid file
	do {
		printf("\nEnter the filepath of the " BLUE "rooms file : " WHITE);
		scanf("%s", roomsFilename);

		roomFile = fopen(roomsFilename, "r"); // attempt access

		// feedback
		if (roomFile == NULL) {
			printf(YELLOW "\t'%s' does not exist. " WHITE "Create it? [Y/N]: ", roomsFilename);
			scanf(" %c", &create);

			// create the file
			if (create == 'Y' || create == 'y') {
				roomFile = fopen(roomsFilename, "w+");
				changeTextColor(GREEN);
				printf("\t%s Created\n", roomsFilename);
			}
		} else {
			changeTextColor(GREEN);
			printf("\tFile located\n");
		}

		changeTextColor(WHITE);
	} while (roomFile == NULL);

	// If the file is not empty. Retrieve the existing data
	if (fscanf(patientFile, "%d", &tmp) != EOF)
	{
		rewind(patientFile); // set file pointer back to the begining of the
							 // file before reading
		i = 0;

		while (!feof (patientFile)) //while not at end-of-file
		{
			fscanf(patientFile, "%d %s %s %s %d %s",
				&patientList[i].id,
				patientList[i].fname,
				patientList[i].lname,
				patientList[i].status,
				&patientList[i].birthYear,
				patientList[i].admissionStatus);

			patientList[i].age = year - patientList[i].birthYear;

			i++;
			*numPatients += 1; // increment the pointer value to reflect in main
		}

		fclose(patientFile); // close the file if we're done with it
	}

	// If the file is not empty. Retrieve the existing data
	if (fscanf(roomFile, "%d", &tmp) != EOF)
	{
		rewind(roomFile);
		i = 0;

		while (!feof (roomFile)) {
			j = 0;
			fscanf (roomFile, "%d %s %d %s %s",
				&roomList[i].roomNum,
				roomList[i].respirator,
				&roomList[i].numBeds,
				roomList[i].personnel,
				patientsIDS);

			// separate all the IDs by comma and place them into
			// roomList[i].patientIds[j]
			char *token = strtok(patientsIDS, ",");

			while (token != NULL) {
				tmp = atoi(token); // convert string to int
				if (tmp != 0){
					roomList[i].patientIds[j] = tmp;
					j++;
				}
				token = strtok(NULL, " ");	// get the next id
			}

			roomList[i].numPatients = j; //patients in the room
			i++;
			*numRooms += 1; // increment the pointer value to reflect in main
		}

		fclose(roomFile);
	}

	printf("\n %d Patients & %d Rooms retrieved\n", *numPatients, *numRooms);
	delay(1000);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// displays the menu
int menu() {
	char opt[10];
	time_t t = time(NULL);
  	struct tm tm = *localtime(&t); //system time and date
	FILE *logFile = fopen("system.log", "a+"); //log file

	clearScreen();
	changeTextColor(WHITE);

	printf ("\n     ____________________________________________");
	printf ("\n    |               MAIN MENU                    |");
	printf ("\n    |--------------------------------------------|");
	printf ("\n    | " GREEN "[1]" WHITE " ADD A PATIENT                          |"
			"\n    | " GREEN "[2]" WHITE " ADD A ROOM                             |"
			"\n    | " GREEN "[3]" WHITE " CHANGE PATIENT STATUS                  |"
			"\n    | " GREEN "[4]" WHITE " AUTO ASSIGN ROOMS                      |"
			"\n    | " GREEN "[5]" WHITE " DISCHARGE A PATIENT                    |"
			"\n    |--------------------------------------------|"
			"\n    | " GREEN "[6]" WHITE " ROOM ADMITTANCE BY AGE GROUP REPORT    |"
			"\n    | " GREEN "[7]" WHITE " ADMITTED PATIENTS LIST REPORT          |"
			"\n    | " GREEN "[8]" WHITE " COVID-19 STATUS REPORT                 |"
			"\n    |--------------------------------------------|"
			"\n    | " GREEN "[9]" WHITE " SAVE AND EXIT                          |"
			"\n    | " GREEN "[0]" WHITE " EXIT WITHOUT SAVE                      |");
	printf ("\n    |____________________________________________|");
	printf ("\n    | Enter Option >                             |");
	printf ("\n    |____________________________________________|");

	gotoxy(22, 18); // move the cursor up
	scanf("%s", opt);

	//append to the log file
	//date, time and option selected
	if (logFile != NULL) 
	{
		fprintf(logFile, "%d-%02d-%02d @ %02d:%02d:%02d | Menu Option %s selected\n", 
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, opt);
		fclose(logFile); // close file
	}
	
	// prevent crash if a string is entered instead of int
    return atoi(opt);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
//creates and adds a new patient to the list
void addPatient(Patient patients[], int *numPatients) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);//get current time & date
	int year = tm.tm_year + 1900; //the current year

	Patient newPatient;
	//numPatients would be the next position, since indexes start at 0
	int newIndex = *numPatients;
	int status;

	clearScreen();
	changeTextColor(CYAN);

	printf("   _________________________________ \n");
	printf("  |  __| |__                        |_ \n");
	printf("  | |__   __|   ADD NEW PATIENT     | |\n");
	printf("  |____|_|__________________________| |\n");
	printf("    |_________________________________|\n\n");
	changeTextColor(WHITE);

	if (*numPatients < MAX_PATIENTS) //35
	{
		//Automatic ID
		if (*numPatients  == 0)
			newPatient.id = 1000212; // starting ID for first patient
		else
			newPatient.id = patients[*numPatients-1].id + 2; // just increase the ID by 2

		printf("\tPatient ID : %d\n", newPatient.id);

		//accept patient info
		printf("\tFirst Name : ");
		scanf("%s", newPatient.fname);

		printf("\tLast Name  : ");
		scanf("%s", newPatient.lname);

		printf("\n\tCovid-19 Status: \n");
		do // validate new status
		{
			printf("\t[1] mild, [2] severe, [3] average, [4] critical, [5] none\n");
			printf("\tselect one: ");
			scanf("%d", &status);
		} while (status != 1 && status != 2 && 
				status != 3 && status != 4 && status != 5);

		if (status == 1)
			strcpy(newPatient.status, "mild");
		else if (status == 2)
			strcpy(newPatient.status, "severe");
		else if (status == 3)
			strcpy(newPatient.status, "average");
		else if (status == 4)
			strcpy(newPatient.status, "critical");
		else
			strcpy(newPatient.status, "none");

		printf("\n\tBirth Year: ");
		scanf("%d", &newPatient.birthYear);

		//calculate age
		newPatient.age = year - newPatient.birthYear;
		//admission status
		strcpy(newPatient.admissionStatus, "admitted");
		
		patients[newIndex] = newPatient; //add the new patient
		*numPatients += 1; //increase the number of patients

		changeTextColor(GREEN);
		printf("\nNew Patient Admitted\n");
	}
	else
	{
		changeTextColor(RED);
		printf("\nMaximum Capacity Reached\n");
	}
	
	delay(1000);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// adds a new room to the rooms array and increases numRooms
void addRoom(Room rooms[], int *numRooms) 
{	
	Room newRoom; //the new room
	char YorN;
	int numBeds, choice, newIndex = *numRooms;

	clearScreen();

	changeTextColor(CYAN);
	printf("   _________________________________\n");
	printf("  |  __| |__                        |_\n");
	printf("  | |__   __|   ADD NEW ROOM        | |\n");
	printf("  |____|_|__________________________| |\n");
	printf("    |_________________________________|\n\n");
	changeTextColor(WHITE);

	//check if limit is exceeded
	if (*numRooms < MAX_ROOMS) //21
	{
		//Automatic ID
		if (*numRooms  == 0)
			newRoom.roomNum = 230; // starting ID for first room
		else
			newRoom.roomNum = rooms[*numRooms-1].roomNum + 1; // new id = last id + 1

		printf("\tRoom Number : %d\n", newRoom.roomNum);

		//accept room info
		printf("\tDoes this room have a respirator [Y/N] : ");
		scanf(" %c", &YorN);

		if (YorN == 'Y' || YorN == 'y')
			strcpy(newRoom.respirator, "respirator");
		else
			strcpy(newRoom.respirator, "none");
		
		do // validate numBeds
		{
			printf("\n\tEnter Number of Beds (1-5): ");
			scanf("%d", &numBeds);
		} while (numBeds > MAX_BEDS_PER_ROOM || numBeds < 1);

		newRoom.numBeds = numBeds;

		printf("\n\tPersonnel: \n");
		do // validate personnel
		{
			printf("\t[1] nurse, [2] doctor, [3] none\n");
			printf("\tselect one: ");
			scanf("%d", &choice);
		} while (choice != 1 && choice != 2 && choice != 3);

		if (choice == 1)
			strcpy(newRoom.personnel, "nurse");
		else if (choice == 2)
			strcpy(newRoom.personnel, "doctor");
		else if (choice == 3)
			strcpy(newRoom.personnel, "none");

		newRoom.numPatients = 0; //no patients were added

		rooms[newIndex] = newRoom; //add the new room
		*numRooms += 1; //increase the number of rooms

		changeTextColor(GREEN);
		printf("\nNew Room Added\n");
	}
	else
	{
		changeTextColor(RED);
		printf("\nMaximum Capacity Reached\n");
	}

	delay(1000);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// update a patient's Covid 19 status
void changePatientStatus(Patient patients[], int numPatients) 
{
	int searchId, i, found = 0;
	int newStatus;

	clearScreen();
	changeTextColor(CYAN);

	printf("   ___________________________________ \n");
	printf("  |                                   |_\n");
	printf("  |   UPDATE PATIENT COVID-19 STATUS  | |\n");
	printf("  |___________________________________| |\n");
	printf("    |___________________________________|\n\n");

	changeTextColor(WHITE);

	printf("Enter the patient ID: ");
	scanf("%d", &searchId);

	// search for the patient by id
	for (i = 0; i < numPatients; i++) {
		if (patients[i].id == searchId) {
			found = 1;
			break; //exit loop
		}
	}

	if (found) {
		changeTextColor(GREEN);
		printf("patient Found!\n\n");
		changeTextColor(WHITE);

		printf(
			"Name            : %s %s\n"
			"Covid-19 Status : %s\n\n",
			patients[i].fname,
			patients[i].lname,
			patients[i].status);

		printf("Possible statuses:\n");

		do // validate new status
		{
			printf("[1] mild, [2] severe, [3] average, [4] critical, [5] none\n\n");
			printf("New Status: ");
			scanf("%d", &newStatus);
		} while (newStatus != 1 && newStatus != 2 && 
				newStatus != 3 && newStatus != 4 && newStatus != 5);

		switch (newStatus) 
		{
		case 1:
			strcpy(patients[i].status, "mild");
			break;

		case 2:
			strcpy(patients[i].status, "severe");
			break;

		case 3:
			strcpy(patients[i].status, "average");
			break;

		case 4:
			strcpy(patients[i].status, "critical");
			break;

		case 5:
			strcpy(patients[i].status, "none");
			break;
		}

		changeTextColor(GREEN);
		printf("Patient %d's status has been updated\n", searchId);
	} else {
		changeTextColor(RED);
		printf("Patient Not Found!\n");
	}

	changeTextColor(WHITE); // reset to default
	delay(1200);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// Automatically assign patients to rooms based on certain criteria
void autoAssignRooms(Patient patients[], Room rooms[], int numpatients, int numrooms)
{
	int i = 0, j = 0;
	char ch;

	clearScreen();
  	changeTextColor(CYAN);
	
	printf("   ____________________________________\n");
	printf("  |     ^                              |_\n");
	printf("  | <<- O ->>   AUTO-ASSIGN FUNCTION   | |\n");
	printf("  |____________________________________| |\n");
	printf("    |____________________________________|\n\n");

	for (i = 0; i < numpatients; i++) // each patient
	{
		//check if patient is not already assigned and not addmitted
		if (getRoomOfPatient (patients[i].id, rooms, numrooms) == 0 && strcmp(patients[i].admissionStatus, "admitted") == 0)
		{
			if (strcmp(patients[i].status, "critical") == 0){
				//search rooms for one that matches the criteria
 				for (j = 0; j < numrooms; j++)
				{
					// room with bed, respirator and doctor
 					if (rooms[j].numPatients < rooms[j].numBeds && strcmp(rooms[j].respirator, "respirator") == 0 
						&& strcmp(rooms[j].personnel, "doctor") == 0)
					{
						printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
						rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
						rooms[j].numPatients ++;
						break; //break to ensure patient is assigned to ONLY one room
					}
 				}
     		}
			else if (strcmp(patients[i].status, "none") == 0){
				strcpy(patients[i].admissionStatus, "discharged");
				//remove them from the room id list too
				removePatientFromRoom(patients[i].id, rooms, numrooms);
				printf(CYAN "[-] Patient %d Discharged\n", patients[i].id);
			}
			else{
				if (patients[i].age > 60)
				{	
     				if (strcmp(patients[i].status, "severe") == 0) {
						//search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed, respirator and/or nurse
							if ((rooms[j].numPatients < rooms[j].numBeds && strcmp(rooms[j].respirator, "respirator") == 0) 
								|| strcmp(rooms[j].personnel, "nurse") == 0)
							{
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
					}
					else if (strcmp(patients[i].status, "mild") == 0){ 
						//search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed
							if (rooms[j].numPatients < rooms[j].numBeds){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
					}
				}
				else if(patients[i].age >= 40 && patients[i].age <= 60)
				{
					if (strcmp(patients[i].status, "average") == 0 || strcmp(patients[i].status, "severe") == 0){
						//search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed and/or nurse
							if (rooms[j].numPatients < rooms[j].numBeds || strcmp(rooms[j].personnel, "nurse") == 0){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
     				}
     				else if (strcmp(patients[i].status, "mild") == 0){
       					//search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed
							if (rooms[j].numPatients < rooms[j].numBeds){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
     				}
				}
				else if (patients[i].age >= 20 && patients[i].age <= 39)
				{
     				if (strcmp(patients[i].status, "severe") == 0) {
						 //search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed and doctor
							if (rooms[j].numPatients < rooms[j].numBeds && strcmp(rooms[j].personnel, "doctor") == 0){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
     				}
     				else if (strcmp(patients[i].status, "mild") == 0 || strcmp(patients[i].status, "average") == 0) {
      					//search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed
							if (rooms[j].numPatients < rooms[j].numBeds){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
    				}
				}
				else if (patients[i].age < 20)
				{
	     			if (strcmp(patients[i].status, "average") == 0 || strcmp(patients[i].status, "severe") == 0){
	       				 //search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed and nurse
							if (rooms[j].numPatients < rooms[j].numBeds && strcmp(rooms[j].personnel, "nurse") == 0){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
	     			}
	     			else if (strcmp(patients[i].status, "mild") == 0){
						 //search rooms for one that matches the criteria
						for (j = 0; j < numrooms; j++)
						{
							// room with bed
							if (rooms[j].numPatients < rooms[j].numBeds){
								printf(GREEN "[-] Patient %d Assigned\n", patients[i].id);
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id;
								rooms[j].numPatients ++;
								break; //break to ensure patient is assigned to ONLY one room
							}
						}
	     			}
	 			}
			} //end patient checks		
		}
		else{
			printf(YELLOW "[-] Patient already assigned or is discharged\n");
		}
	} //endfor

	if (numpatients == 0)
	{
		changeTextColor(YELLOW);
		printf("NO PATIENTS IN THE SYSTEM! ADD PATIENTS TO DO THIS ACTION\n");
	}
	if (numrooms == 0)
	{
		changeTextColor(YELLOW);
		printf("NO ROOMS IN THE SYSTEM! ADD ROOMS TO DO THIS ACTION\n");
	}
	else{
		changeTextColor(WHITE);
		printf("\nIf a patient was not assigned, consider adding more rooms\n");
	}

	changeTextColor (WHITE); // reset to default
	printf("\nInput any key to continue: ");
	scanf(" %c", &ch);
	
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// discharge a patient from the hospital
void dischargePatient(Patient patients[], int numPatients, Room rooms[], int numrooms)
{
  	char opt;
  	int searchId, i, found = 0;
    
	clearScreen();
    changeTextColor(CYAN);
	printf("   ____________________________________\n");
	printf("  |  ______                            |_\n");
	printf("  | |______|   DISCHARGE PATIENT       | |\n");
	printf("  |____________________________________| |\n");
	printf("    |____________________________________|\n\n");
	changeTextColor(WHITE);

	printf("Enter Patient ID: ");
	scanf("%d", &searchId); //ID
	
	// search for the patient by id
	for (i = 0; i < numPatients; i++) 
  	{
		if (patients[i].id == searchId) 
    	{
			found = 1;
			break; //exit loop
		}
	}

	if (found) {
		changeTextColor(GREEN);
		printf("patient Found!\n\n");
		changeTextColor(WHITE);

		printf(
			"Name            : %s %s\n"
			"Covid-19 Status : %s\n"
			"Admitted?       : %s\n\n",
			patients[i].fname, patients[i].lname,
			patients[i].status, patients[i].admissionStatus);

		// check that the patient isnt already discharged
		if (strcmp(patients[i].admissionStatus, "admitted") == 0)
		{
			printf("Are you sure you want to discharge this patient?\n");
			printf("[Y] or [N]: ");
			scanf(" %c", &opt);
			
			if (opt == 'Y' || opt == 'y')
			{
				strcpy(patients[i].admissionStatus, "discharged");
				removePatientFromRoom(searchId, rooms, numrooms);
				changeTextColor(GREEN);
				printf("\nPATIENT %d DISCHARGED\n", searchId);
			}
			else
			{
				changeTextColor(YELLOW);
				printf("\nOperation Cancelled!\n");
			}
		}
		else{
			changeTextColor(YELLOW);
			printf("Patient already discharged\n");
		}
	} else {
		changeTextColor(RED);
		printf("Patient Not Found!\n");
	}

	changeTextColor(WHITE); // reset to default
	delay(1200);
}


// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// prints a report on the number within an age group
void attendanceByAgeReport(Patient patients[], Room rooms[], int numPatients, int numRooms)
{
	int over60, age40To60, age20To39, under20, i;
	FILE *filePtr;
	char ch, filename[50];
	over60 = age40To60 = age20To39 = under20 = 0;

    clearScreen();
    changeTextColor(CYAN);
	printf(" _  _____  _____________________\n");
	printf("|  | --- |     ROOM ATTENDANCE  |\n");
	printf("|  | ### ||    BY AGE           |\n");
	printf("|  |_____||    REPORT           |\n");
	printf("|__ |_____|    _________________|\n\n");
	changeTextColor(WHITE);

	if (numPatients != 0)
	{
		for (i = 0; i < numPatients; i++){
			// calculate counters
			if (patients[i].age > 60)
				over60 ++;
			else if (patients[i].age >= 40 && patients[i].age < 60)
				age40To60 ++;
			else if (patients[i].age >= 20 && patients[i].age < 40)
				age20To39 ++;
			else
				under20 ++;
		}

		// DISPLAY THE BAR CHART
		// chart title
		printf("NUMBER OF PATIENTS BY AGE GROUP\n");

		printf ("\nOver 60 | "); //print the category/label
		for (i = 0; i < over60; i++)
			printf (GREEN "###" WHITE);
		
		printf ("\n        |\n40 To 60| "); //print the category/label
		for (i = 0; i < age40To60; i++)
			printf (GREEN "###" WHITE);
			
		printf ("\n        |\n20 To 40| "); //print the category/label
		for (i = 0; i < age20To39; i++)
			printf (GREEN "###" WHITE);
		
		printf ("\n        |\nUnder 20| "); //print the category/label
		for (i = 0; i < under20; i++)
			printf (GREEN "###" WHITE);
		
		//print the bottom axis
		printf ("\n        |");
		for (i = 0; i <= numPatients; i++)
			printf ("___");

		printf ("\n         ");
		for (i = 0; i <= numPatients; i++)
			printf("%-3d", i);

		printf ("\n\nEnter any key to continue: ");
		scanf(" %c", &ch);
		clearScreen(); //to save screen space

		//BASIC PRINTOUT
		printf (WHITE "PATIENTS OVER  60 ______" GREEN " %d\n", over60);
		printf (WHITE "PATIENTS 40 TO 60 ______" GREEN " %d\n", age40To60);
		printf (WHITE "PATIENTS 20 TO 40 ______" GREEN " %d\n", age20To39);
		printf (WHITE "PATIENTS UNDER 20 ______" GREEN " %d\n", under20);

		changeTextColor(WHITE);
		
		printf("\nSave The Report Data To a File? [Y/N]: ");
		scanf(" %c", &ch);

		if (ch == 'Y' || ch == 'y')
		{
			printf("\nEnter Filename (and extension): ");
			scanf ("%s", filename);
			
			filePtr = fopen(filename, "w"); //the file is opened

			if (filePtr != NULL)
			{
				//data is printed to the file
				fprintf (filePtr, "PATIENTS OVER 60  ______ %d\n", over60);
				fprintf (filePtr, "PATIENTS 40 TO 60 ______ %d\n", age40To60);
				fprintf (filePtr, "PATIENTS 20 TO 40 ______ %d\n", age20To39);
				fprintf (filePtr, "PATIENTS UNDER 20 ______ %d\n", under20);
				fclose (filePtr); //the file is closed
				printf (GREEN "FILE CREATED !\n");
			}
			else{
				//message is printed if the file is not created
				changeTextColor(RED);
				printf("'%s' couldn't be accessed.\n", filename);
			}
		}
		else
		{
			changeTextColor(YELLOW);
			printf("Operation Cancelled!\n");
		}
	}
	else
	{
		changeTextColor(YELLOW);
		printf("NO PATIENTS IN THE SYSTEM! \n ADD PATIENTS TO VIEW REPORTS\n");
	}

	changeTextColor(WHITE); // reset to default
	delay(1200);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// prints a report of all patients 
void admittedPatientsListReport(Patient patients[], Room rooms[], int numPatients, int numRooms)
{
	int p, min_idx, room;
	char filename[50], ch;
	FILE *filePtr;
	Patient patientsClone[MAX_PATIENTS];

    clearScreen();
    changeTextColor(CYAN);
	printf(" _  _____  _______________\n");
	printf("|  | --- |     ADMITTED   |\n");
	printf("|  | ### ||    PATIENTS   |\n");
	printf("|  |_____||    REPORT     |\n");
	printf("|__ |_____|    ___________|\n\n");
	changeTextColor(WHITE);

	if (numPatients != 0)
	{
		//clone patients array
		for (int i = 0; i < numPatients; i++){
			patientsClone[i] = patients[i];
		}

		//order patientIds in ascending order (selection sort)
 		// One by one move boundary of unsorted subarray
		for (int i = 0; i < numPatients - 1; i++) {

			// Find the minimum element in unsorted array
			min_idx = i;
			for (int j = i + 1; j < numPatients; j++){
				if (patientsClone[j].id < patientsClone[min_idx].id)
					min_idx = j;
      		}
			// Swap the found minimum element
			// with the first element
			swap(&patientsClone[min_idx], &patientsClone[i]);
		}
		
		// print list of information
		for (int i = 0; i < numPatients; i++){
			// get their room number
			room = getRoomOfPatient(patientsClone[i].id, rooms, numRooms);

			//print patient data
			printf("ID    | "GREEN"%d\n"WHITE, patientsClone[i].id);
			printf("Name  | %s %s\n", patientsClone[i].fname, patientsClone[i].lname);
			//ternary operator (if-else alternative)
			room != 0? printf("Room #| %d\n", room): printf("Room #| None\n");
			printf("Status| %s\n\n", patientsClone[i].status);
		}

		// file save option
		printf("\nSave This Report To a File? [Y/N]: ");
		scanf(" %c", &ch);

		if (ch == 'Y' || ch == 'y')
		{
			printf("\nEnter Filename (and extension): ");
			scanf ("%s", filename);
			
			filePtr = fopen(filename, "w"); //the file is opened

			if (filePtr != NULL)
			{
				//data is printed to the file
				for (int i = 0; i < numPatients; i++)
				{ 
					// get their room number
					room = getRoomOfPatient(patientsClone[i].id, rooms, numRooms);

					fprintf(filePtr, "ID    : %d\n", patientsClone[i].id);
					fprintf(filePtr, "Name  : %s %s\n", patientsClone[i].fname, patientsClone[i].lname);
					//ternary operator (if-else alternative)
					room != 0? fprintf(filePtr, "Room #: %d\n", room): fprintf(filePtr, "Room #: None\n");
					fprintf(filePtr, "Status: %s\n\n", patientsClone[i].status);
				}

				fclose (filePtr); //the file is closed
				printf (GREEN "FILE CREATED !\n");
			}
			else{
				//message is printed if the file is not created
				changeTextColor(RED);
				printf("'%s' couldn't be accessed.\n", filename);
			}
		}
		else
		{
			changeTextColor(YELLOW);
			printf("Operation Cancelled!\n");
		}
	}
	else
	{
		changeTextColor(YELLOW);
		printf("NO PATIENTS IN THE SYSTEM! \n ADD PATIENTS TO VIEW REPORTS\n");
	}

	//printf("Enter");
  	changeTextColor(WHITE); // reset to default
	delay(1200);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// prints a report on the number of patients with a covid status
void covid19StatusReport(Patient patients[], int numPatients)
{
	int none = 0, mild = 0, severe = 0, critical = 0, average = 0, i;
	char ch, filename[25], choice;
	FILE *filePtr;

    clearScreen();
    changeTextColor(CYAN);
	printf(" _  _____  _______________\n");
	printf("|  | --- |     COVID-19   |\n");
	printf("|  | ### ||    STATUS     |\n");
	printf("|  |_____||    REPORT     |\n");
	printf("|__ |_____|    ___________|\n\n");
	changeTextColor(WHITE);

	if (numPatients != 0){
		// GET TOTALS
		for (i = 0; i < numPatients; i++)
		{
			if (strcmp (patients[i].status, "mild") == 0)
				mild++;
			else if (strcmp (patients[i].status, "severe") == 0)
				severe++;
			else if (strcmp (patients[i].status, "critical") == 0)
				critical++;
			else if(strcmp (patients[i].status, "average") == 0)
				average++;
			else
				none++;
		}

		// DISPLAY THE BAR CHART
		// chart title
		printf("NUMBER OF PATIENTS BY COVID STATUS\n");

		printf ("\nCritical| "); //print the category/label
		for (i = 0; i < critical; i++)
			printf (GREEN "###" WHITE);
		
		printf ("\n        |\nMild    | "); //print the category/label
		for (i = 0; i < mild; i++)
			printf (GREEN "###" WHITE);
			
		printf ("\n        |\nSevere  | "); //print the category/label
		for (i = 0; i < severe; i++)
			printf (GREEN "###" WHITE);
		
		printf ("\n        |\nAverage | "); //print the category/label
		for (i = 0; i < average; i++)
			printf (GREEN "###" WHITE);

		printf ("\n        |\nNone    | "); //print the category/label
		for (i = 0; i < none; i++)
			printf (GREEN "###" WHITE);
		
		//print the bottom axis
		printf ("\n        |");
		for (i = 0; i <= numPatients; i++)
			printf ("___");

		printf ("\n         ");
		for (i = 0; i <= numPatients; i++)
			printf("%-3d", i);

		printf ("\n\nEnter any key to continue: ");
		scanf(" %c", &ch);
		clearScreen(); //to save screen space

		//BASIC PRINTOUT
		printf (WHITE "MILD CASES __________" GREEN " %d\n", mild);
		printf (WHITE "SEVERE CASES ________" GREEN " %d\n", severe);
		printf (WHITE "AVERAGE CASES _______" GREEN " %d\n", average);
		printf (WHITE "CRITICAL CASES ______" GREEN " %d\n", critical);
		printf (WHITE "NON COVID CASES _____" GREEN " %d\n", none);

		changeTextColor(WHITE);
		
		printf("\nSave This Report To a File? [Y/N]: ");
		scanf(" %c", &choice);

		if (choice == 'Y' || choice == 'y')
		{
			printf("\nEnter Filename (and extension): ");
			scanf ("%s", filename);
			
			filePtr = fopen(filename, "w"); //the file is opened

			if (filePtr != NULL)
			{
				//data is printed to the file
				fprintf (filePtr, "MILD CASES __________ %d\n", mild);
				fprintf (filePtr, "SEVERE CASES ________ %d\n", severe);
				fprintf (filePtr, "AVERAGE CASES _______ %d\n", average);
				fprintf (filePtr, "CRITICAL CASES ______ %d\n", critical);
				fprintf (filePtr, "NON COVID CASES _____ %d\n", none);
				fclose (filePtr); //the file is closed
				printf (GREEN "FILE CREATED !\n");
			}
			else{
				//message is printed if the file is not created
				changeTextColor(RED);
				printf("'%s' couldn't be accessed.\n", filename);
			}
		}
		else
		{
			changeTextColor(YELLOW);
			printf("Operation Cancelled!\n");
		}
	}
	else{
		changeTextColor(YELLOW);
		printf("NO PATIENTS IN THE SYSTEM! \n ADD PATIENTS TO VIEW REPORTS\n");
	}

	changeTextColor(WHITE); // reset to default
	delay(1200);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// returns the roomnum that a patient is assigned to, returns 0 if no room was found
int getRoomOfPatient (int patientId, Room rooms[], int numrooms){
	int roomNum = 0;

	for (int i = 0; i < numrooms; i++) //each room
	{
		for (int j = 0; j < rooms[i].numPatients; j++) // each patient in room
		{
			if (rooms[i].patientIds[j] == patientId)
			{
				roomNum = rooms[i].roomNum;
				break;
			}
		}
	}

	return roomNum;
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// removes a patient from the room he/she is assigned
void removePatientFromRoom (int patientId, Room rooms[], int numrooms){
	// find the room they belong to
	int roomNum = getRoomOfPatient(patientId, rooms, numrooms);
	int roomIndex, i, j;
	int newPatientIds[MAX_PATIENTS_PER_ROOM];
	
	if (roomNum != 0) // check if they were assigned
	{
		//determine the index of the room in the rooms[] array
		for (roomIndex = 0; roomIndex < numrooms; roomIndex++){
			if (rooms[roomIndex].roomNum == roomNum)
			{
				j = 0;
				// populate the new patientList without the patient to be removed
				for (i = 0; i < rooms[roomIndex].numPatients; i++)
				{
					if (rooms[roomIndex].patientIds[i] != patientId){
						newPatientIds[j] = rooms[roomIndex].patientIds[i];
						j++;
					}
				}
				rooms[roomIndex].numPatients --; //decrement
				break;
			}
		}
		
		// assign the new values to the patientIds of the Room
		for (i = 0; i < rooms[roomIndex].numPatients; i++)
			rooms[roomIndex].patientIds[i] = newPatientIds[i];
	}
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// Saves the data to file
void saveChanges(char *patientsfilename, char *roomsfilename,
	Patient patients[], Room rooms[],
	int numPatients, int numRooms) 
{
	// open files to overwrite old data with the new
	FILE *patientFile = fopen(patientsfilename, "w");
	FILE *roomFile = fopen(roomsfilename, "w");
	int j = 0, i = 0, k;
	char id[9] = "", idList[40] = "";

	clearScreen();

	// write patients to file
	for (i = 0; i < numPatients; i++) {
		fprintf(
			patientFile,
			"%d %s %s %s %d %s",
			patients[i].id,
			patients[i].fname,
			patients[i].lname,
			patients[i].status,
			patients[i].birthYear,
			patients[i].admissionStatus);
			
		// do not add newline to the end of file
		if (i < numPatients - 1)
			fprintf(patientFile, "\n");
	}

	// write rooms to file
	for (i = 0; i < numRooms; i++) {
		j = 0, k = 0; // j is used to iterate through the patient ids
		strcpy(idList, ""); // reset

		// get the different patient IDs
		// and put them in the form "id,id,id"
		do {
			if (rooms[i].numPatients != 0)
			{
				sprintf(id, "%d,", rooms[i].patientIds[j]);
				strcat(idList, id);
				k++;
				j++;
			} else {
				strcpy(idList, "none"); // no patients
				break;
			}
		} while (rooms[i].numPatients > k);

		fprintf(
			roomFile,
			"%d %s %d %s %s",
			rooms[i].roomNum,
			rooms[i].respirator,
			rooms[i].numBeds,
			rooms[i].personnel,
			idList);

		// do not add newline to the end of file
		if (i < numRooms - 1)
			fprintf(roomFile, "\n");
	}

	// close the files, we're done with them
	fclose(patientFile); 
	fclose(roomFile);

	changeTextColor(GREEN);
	printf("\nDATA SUCCESSFULLY SAVED\n");
	
	delay(1000);
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
//displays an exit animation
void exitProgram() 
{
	clearScreen();
	changeTextColor(RED);

	printf ("  _____  ____   ____  _____  ______     ________  \n");
	printf (" / ____|/ __ \\ / __ \\|  __ \\|  _ \\ \\   / /  ____| \n");
	printf ("| |  __| |  | | |  | | |  | | |_) \\ \\_/ /| |__    \n");
	printf ("| | |_ | |  | | |  | | |  | |  _ < \\   / |  __|   \n");
	printf ("| |__| | |__| | |__| | |__| | |_) | | |  | |____  \n");
	printf (" \\_____|\\____/ \\____/|_____/|____/  |_|  |______| \n");

	printf("\n   See you soon!\n");
	exit(0); // terminates program
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// allows us to change the color of the text
void changeTextColor(char *color) {
	printf(color, "");
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// clears the screen using the command specific to the OS
/// This ensures that it works on both MacOS and Windows compilers
void clearScreen() 
{
	// ifdef means 'if defined'.
	#ifdef _WIN32
		system("cls"); // clear screen on windows (devc,codeblocks)
	#else
		system("clear"); // clear screen on Mac and online compilers (linux)
	#endif
}

// !!!!!!!! COMPLETED FUNCTION: DON'T MODIFY WITHOUT GROUP CONSENT !!!!!!!!!
// pauses the program for milliseconds
void delay (int milliseconds)
{
    long pause;
    clock_t now, then;

	// use system time to cause a delay
    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ( (now - then) < pause )
        now = clock();
}

void swap(Patient* xp, Patient* yp)
{
    Patient temp = *xp;
    *xp = *yp;
    *yp = temp;
}
// NOTE: put function definitions in the same order the prototypes appear at the top of the file