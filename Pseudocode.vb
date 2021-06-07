DECLARE CONSTANT MAX_PATIENTS = 35
DECLARE CONSTANT MAX_ROOMS = 21
DECLARE CONSTANT MAX_BEDS_PER_ROOM = 5
DECLARE CONSTANT MAX_PATIENTS_PER_ROOM = 5

RECORD Patient
	DECLARE id as INTEGER
	DECLARE fname as STRING
	DECLARE lname as STRING
	DECLARE status as STRING
	DECLARE birthYear as INTEGER
	DECLARE age as INTEGER
	DECLARE admissionStatus as STRING
ENDRECORD

RECORD Room
	DECLARE roomNum as INTEGER
	DECLARE respirator as STRING
	DECLARE numBeds as INTEGER
	DECLARE personnel as STRING
	DECLARE patientIds[MAX_PATIENTS_PER_ROOM] as INTEGER
ENDRECORD


START
	DECLARE patients[MAX_PATIENTS] as Patient
	DECLARE rooms[MAX_ROOMS] as Room
	DECLARE patientsfilename, roomsfilename as STRING
	DECLARE numPatients = 0, numRooms = 0 as INTEGER
	DECLARE choice as INTEGER

	CALL populateArraysFromFile ( args: patientsfilename, roomsfilename, patients, rooms, numPatients, numRooms)

	choice = CALL menu()

	WHILE True DO
		DO CASE of choice
			CASE 1:
				CALL addPatient(patients, &numPatients)
			
			CASE 2:
				CALL addRoom(rooms, &numRooms)

			CASE 3:
				CALL changePatientStatus(patients, numPatients)

			CASE 4:
				CALL autoAssignRooms(patients, rooms, numPatients, numRooms)

			CASE 5:
				CALL dischargePatient(patients, numPatients, rooms, numRooms)

			CASE 6:
				CALL attendanceByAgeReport(patients, rooms, numPatients, numRooms)

			CASE 7:
				CALL admittedPatientsListReport(patients, rooms, numPatients, numRooms)

			CASE 8:
				CALL covid19StatusReport(patients, numPatients)

			CASE 9:
				CALL saveChanges(patientsfilename, roomsfilename, patients, rooms, numPatients, numRooms)
				STOP

			CASE 0:
				PRINT "GOODBYE"
				STOP
			
			OTHER:
				PRINT "INVALID OPTION PLEASE RE-ENTER"
		ENDCASE 

		choice = CALL menu()
	ENDWHILE
STOP


FUNCTION populateArraysFromFile (patientsFilename, roomsFilename, patientList[], roomList[], *numPatients, *numRooms) 
	DECLARE patientFile, roomFile as FILE
	DECLARE i = 0, j = 0, year as INTEGER
	DECLARE create as CHARACTER
	DECLARE patientsIDS as STRING
	
	year = TIME.year

	REPEAT
		PRINT "Enter the filepath of the patients file : "
		READ patientsFilename

		patientFile = OPEN FILE (patientsFilename, for reading)

		IF patientFile = NULL THEN
			PRINT "File does not exist. Create it? [Y/N]: "
			READ create
			
			IF create = 'Y' OR  create = 'y' THEN
				patientFile = CREATE FILE (patientsFilename, for reading)
				PRINT "Created ", patientsFilename
			ENDIF
		ELSE
			PRINT "File located "
		ENDIF
	UNTIL patientFile <> NULL

	REPEAT
		PRINT "Enter the filepath of the rooms file : "
		READ roomsFilename

		roomFile = OPEN FILE (roomsFilename, for reading)

		IF (roomFile == NULL) 
			PRINT "File does not exist. Create it? [Y/N]: "
			READ create

			IF create = 'Y' OR create = 'y' THEN
				roomFile = CREATE FILE (roomsFilename, for reading)
				PRINT "Created", roomsFilename
			ENDIF
		ELSE 
			PRINT "File located"
		ENDIF
	UNTIL roomFile <> NULL

	IF NOT FILE_EMPTY (patientFile) THEN
		i = 0

		WHILE NOT END_OF_FILE (patientFile) DO
			READ patientList[i].id,
				patientList[i].fname,
				patientList[i].lname,
				patientList[i].status,
				patientList[i].birthYear,
				patientList[i].admissionStatus FROM FILE patientFile

			patientList[i].age = year - patientList[i].birthYear

			i = i + 1
			*numPatients = *numPatients + 1
		ENDWHILE
    
		CLOSE FILE patientFile
	ENDIF 

	IF NOT FILE_EMPTY (roomFile) THEN
		i = 0

		WHILE NOT END_OF_FILE (roomFile) DO
			j = 0
			READ roomList[i].roomNum,
				roomList[i].respirator,
				roomList[i].numBeds,
				roomList[i].personnel,
				patientsIDS FROM FILE roomFile

			roomList[i].patientIds = SPLIT patientsIDS BY ","

			i = i + 1
			*numRooms = *numRooms + 1
		ENDWHILE

		CLOSE FILE roomFile
	ENDIF
ENDFUNCTION

FUNCTION menu ()
	DECLARE opt as INTEGER
	DECLARE tm as TIME
	DECLARE dt as DATE
	DECLARE logFile as FILE

	logFile = OPEN FILE "system.log", for appending

	PRINT "MAIN MENU"
	PRINT "[1] ADD A PATIENT"
	PRINT "[2] ADD A ROOM"
	PRINT "[3] CHANGE PATIENTS STATUS"
	PRINT "[4] AUTO ASSIGN ROOMS"
	PRINT "[5] DISCHARGE A PATIENT"
	PRINT "[6] ROOM ADMITTANCE BY AGE GROUP REPORT"
	PRINT "[7] ADMITTED PATIENTS LIST REPORT"
	PRINT "[8] COVID-19 STATUS REPORT"
	PRINT "[9] SAVE AND EXIT"
	PRINT "[0] EXIT WITHOUT SAVE"
	PRINT "ENTER OPTION > "
    READ opt 

	IF logFile <> NULL THEN
		WRITE dt, " @ ", tm, " | Menu Option ", opt ," selected" TO FILE logFile
		CLOSE FILE logFile
	ENDIF

	RETURN opt
ENDFUNCTION

FUNCTION addPatient( patients[], *numPatients)

	DECLARE newPatient as Patient
	DECLARE newIndex, status as INTEGER
  	newIndex = *numPatients

	PRINT "ADD NEW PATIENT"

    IF *numPatients < MAX_PATIENTS THEN 
        IF *numPatients = 0 THEN
          	newPatient.id = 1000212
        ELSE
          	newPatient.id = patients[*numPatients - 1].id + 2
        ENDIF

		PRINT "First Name : "
		READ newPatient.fname

		PRINT "Last Name : "
		READ newPatient.lname

		PRINT "Covid-19 Status: "
        REPEAT
			PRINT "[1] mild, [2] severe, [3] average, [4] critical, [5] none"
			PRINT "select one: "
			READ status
        UNTIL status = 1 OR status = 2 OR status = 3 OR status = 4 OR status = 5

        IF status == 1 THEN
          	newPatient.status = "mild"
        ELSE IF status == 2 THEN
          	newPatient.status = "severe"
        ELSE IF status == 3 THEN
          	newPatient.status = "average"
		ELSE IF status == 4 THEN
          	newPatient.status = "critical"
        ELSE
          	newPatient.status = "none"
        ENDIF 

		PRINT "Birth Year: "
		READ newPatient.birthYear

		newPatient.admissionStatus = "admitted"

		patients[newIndex] = newPatient
		*numPatients = *numPatients + 1

      	PRINT "New Patient Admitted" 
    ELSE 
      	PRINT "Maximum Capacity Reached"
    ENDIF 
ENDFUNCTION
		
FUNCTION addRoom (rooms[], *numRooms) 
	DECLARE newRoom as Room
	DECLARE YorN as CHARACTER
	DECLARE numBeds, choice, newIndex as INTEGER
  	newIndex = *numRooms

	PRINT "ADD NEW ROOM"

    IF *numRooms < MAX_ROOMS THEN
        IF *numRooms  = 0 THEN
			newRoom.roomNum = 230
		ELSE
			newRoom.roomNum = rooms[*numRooms - 1].roomNum + 1
        ENDIF 
        
		PRINT "Does this room have a respirator [Y/N] : "
		READ YorN

        IF YorN = 'Y' OR YorN = 'Y' THEN
          	newRoom.respirator = "respirator"
        ELSE
        	newRoom.respirator = "none"
        ENDIF 
        
        REPEAT
			PRINT " Enter Number of Beds (1-5): "
			READ numBeds
        UNTIL numBeds <= MAX_BEDS_PER_ROOM AND numBeds >= 1

      	newRoom.numBeds = numBeds

      	PRINT "Personnel: "
        REPEAT
			PRINT "[1] nurse, [2] doctor, [3] none"
			PRINT "select one: "
			READ choice
        UNTIL choice = 1 OR choice = 2 OR choice = 3

        IF choice = 1 THEN
          	newRoom.personnel = "nurse"
        ELSE IF choice = 2 THEN 
          	newRoom.personnel = "doctor"
        ELSE 
          	newRoom.personnel = "none"
        ENDIF 

      	newRoom.patientIds[0] = 0

      	rooms[newIndex] = newRoom
      	*numRooms = *numRooms + 1

      	PRINT "New Room Added"
    ELSE
      	PRINT "Maximum Capacity Reached"
    ENDIF
ENDFUNCTION

FUNCTION changePatientStatus(patients[], numPatients)
	DECLARE searchId, i = 0, found = 0, newStatus as INTEGER

	PRINT "Enter the patient ID: "
	READ searchId

    FOR  i < numPatients DO 
		IF patients[i].id == searchId THEN
			found = 1
		ENDIF
    ENDFOR

    IF found THEN
		PRINT "patient Found!"
		PRINT "Name            :", patients[i].fname, patients[i].lname,
		PRINT "Covid-19 Status :", patients[i].status
		
		PRINT "Possible statuses: "

		REPEAT 
			PRINT "[1] mild, [2] severe, [3] average, [4] critical, [5] none"
			PRINT "New Status: "
			READ newStatus
		UNTIL newStatus = 1 OR newStatus = 2 OR newStatus = 3 OR newStatus = 4 OR newStatus = 5

		IF newStatus == 1 THEN
			patients[i].status = "mild"
		ELSE IF newStatus == 2 THEN 
			patients[i].status = "severe"
		ELSE IF newStatus == 3 THEN
			patients[i].status = "average"
		ELSE IF newStatus == 4 THEN
			patients[i].status = "critical"
		ELSE
          	patients[i].status = "none"
		ENDIF

		PRINT "Patient status has been updated "
    ELSE  
      	PRINT "Patient Not Found!"
    ENDIF 
ENDFUNCTION

FUNCTION autoAssignRooms(patients[], rooms[], numPatients, numRooms)

	DECLARE i = 0, j = 0 as INTEGER

	PRINT "AUTO ASSIGN"

	FOR i = 0 TO numpatients DO
		IF getRoomOfPatient (patients[i].id, rooms, numrooms) == 0 AND patients[i].admissionStatus == "admitted" THEN

  			IF patients[i].status == "critical" THEN
				
        		FOR j = 0 TO numrooms DO
				
					IF rooms[j].numPatients < rooms[j].numBeds AND rooms[j].respirator == "respirator" AND rooms[j].personnel == "doctor" THEN
						PRINT "Patient ", patients[i].id, " Assigned"
  						rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
  						rooms[j].numPatients = rooms[j].numPatients + 1
						EXIT LOOP
					ENDIF
				ENDFOR
			ELSE IF patients[i].status == "none" THEN
				patients[i].admissionStatus = "discharged"
				removePatientFromRoom(patients[i].id, rooms, numrooms)
				PRINT "Patient ", patients[i].id, " Discharged"
			ELSE
				IF patients[i].age > 60 THEN
    				IF patients[i].status == "severe" THEN
						
						FOR j = 0 TO numrooms DO
							IF (rooms[j].numPatients < rooms[j].numBeds AND rooms[j].respirator == "respirator") OR rooms[j].personnel == "nurse" THEN

								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
					ELSE IF patients[i].status == "mild" THEN 
						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
					ENDIF
				ELSE IF patients[i].age >= 40 AND patients[i].age <= 60 THEN
				
					IF patients[i].status == "average" OR patients[i].status == "severe" THEN
						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds OR rooms[j].personnel == "nurse" THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR

    			ELSE IF patients[i].status == "mild" THEN
						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
    				ENDIF
				ELSE IF patients[i].age >= 20 AND patients[i].age <= 39 THEN

    			IF patients[i].status == "severe" THEN
						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds AND rooms[j].personnel == "doctor" THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
					ELSE IF patients[i].status == "mild" OR patients[i].status == "average" THEN

						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
   					ENDIF
				ELSE IF patients[i].age < 20 THEN

	     			IF strcmp(patients[i].status == "average" OR patients[i].status == "severe" THEN

						FOR j = 0 TO numrooms DO 
							IF rooms[j].numPatients < rooms[j].numBeds AND rooms[j].personnel == "nurse" THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
	     			ELSE IF patients[i].status == "mild" THEN

						FOR j = 0 TO numrooms DO
							IF rooms[j].numPatients < rooms[j].numBeds THEN
								PRINT "Patient ", patients[i].id, " Assigned"
								rooms[j].patientIds[rooms[j].numPatients] = patients[i].id
								rooms[j].numPatients = rooms[j].numPatients + 1
								EXIT LOOP
							ENDIF
						ENDFOR
	     			ENDIF
	 			ENDIF
			ENDIF
		ELSE
			PRINT "Patient already assigned or is discharged"
		ENDIF
	ENDFOR

	IF numpatients == 0 THEN
		PRINT "NO PATIENTS IN THE SYSTEM! ADD PATIENTS TO DO THIS ACTION"
	ENDIF
ENDFUNCTION


FUNCTION dischargePatient(patients[], numPatients, rooms[], numRooms)

	DECLARE opt as INTEGER
	DECLARE searchId , i , found = 0 as INTEGER

	PRINT "DISCHARGE PATIENT"

	PRINT "Enter Patient ID:  "
	READ searchId

	FOR i = 0 TO numPatients DO
		IF patients[i].id == searchId THEN
			found = 1
			EXIT LOOP 
		ENDIF
	ENDFOR

  	IF found == 1 THEN 
		PRINT "patient found"
		PRINT "Name            : " , patients[i].fname, patients[i].lname
		PRINT "Covid-19 Status : ", patients[i].status
		PRINT "Admitted?       : ", patients[i].admissionStatus

		IF patients[i].admissionStatus == "admitted" THEN 
			PRINT "Are you sure you want to discharge this patient?"
			PRINT "[Y] or [N]: "
			READ opt

			IF opt == "y" OR opt =="y" THEN 
				patients[].admissionStatus = "discharged"
				CALL removePatientFromRoom(searchId, rooms, numrooms)
				PRINT "PATIENT", searchId, " DISCHARGED"
			ELSE
				PRINT "Operation Cancelled!"
			ENDIF
		ELSE
			PRINT "Patient already discharged"
		ENDIF
	ELSE
		PRINT "Patient Not Found! "
  	ENDIF
ENDFUNCTION


FUNCTION attendanceByAgeReport(patients[], rooms[], numPatients, numRooms)

  	DECLARE over60=0, age40To60=0, age20To39=0, under20=0, i as INTEGER
	DECLARE filePtr as FILE
	DECLARE ch as CHARACTER
	DECLARE filename as STRING

  	IF numPatients <> 0 THEN
		FOR i = 0 to numPatients DO 
			IF patients[i].age > 60 THEN
				over60 = over60 + 1
			ELSE IF patients[i].age >= 40 AND patients[i].age < 60 THEN
				age40To60 = age40To60 + 1
			ELSE IF patients[i].age >= 20 AND patients[i].age < 40 THEN
				age20To39 = age20To39 + 1
			ELSE
				under20 = under20 + 1
			ENDIF
		ENDFOR

		PRINT "NUMBER OF PATIENTS BY COVID STATUS "
		PRINT  "PATIENTS OVER  60 ______", over60
		PRINT  "PATIENTS 40 TO 60 ______", age40To60
		PRINT  "PATIENTS 20 TO 40 ______", age20To39
		PRINT  "PATIENTS UNDER 20 ______", under20
		
		PRINT "Save The Report Data To a File? [Y/N]: "
		READ ch

		IF ch == 'Y' OR ch == 'y' THEN
			PRINT "Enter Filename (and extension): "
			READ filename
			
			filePtr = CREATE FILE (filename, for writing)

			IF filePtr <> NULL THEN 
				FILE PRINT filePtr, "PATIENTS OVER 60  ______ ", over60
				FILE PRINT filePtr, "PATIENTS 40 TO 60 ______ ", age40To60
				FILE PRINT filePtr, "PATIENTS 20 TO 40 ______ ", age20To39
				FILE PRINT filePtr, "PATIENTS UNDER 20 ______ ", under20
				CLOSE FILE filePtr 
				PRINT "FILE CREATED !"
			ELSE
				PRINT "couldn't be accessed.", filename
			ENDIF
		ELSE
			PRINT "Operation Cancelled!"
		ENDIF 
    ELSE
		PRINT "NO PATIENTS IN THE SYSTEM!"
		PRINT "ADD PATIENTS TO VIEW REPORTS"
    ENDIF
ENDFUNCTION

FUNCTION admittedPatientsListReport(patients[], rooms[], numPatients, numRooms)

	DECLARE p, min_idx, room as INTEGER
	DECLARE ch as CHARACTER
	DECLARE filename as STRING
	DECLARE filePtr as FILE
	DECLARE patientsClone[MAX_PATIENTS] as Patient

	IF numPatients <> 0 THEN
		FOR  i = 0 TO numPatients DO
			patientsClone[i] = patients[i]
		ENDFOR

		FOR i = 0 TO numPatients - 1 DO
			min_idx = i
			FOR j = i + 1 TO numPatients DO
				IF patientsClone[j].id < patientsClone[min_idx].id THEN
					min_idx = j
      			ENDIF
			ENDFOR
			
			CALL swap(&patientsClone[min_idx], &patientsClone[i])
		ENDFOR
		
		FOR i = 0 TO numPatients DO
			room = CALL getRoomOfPatient(patientsClone[i].id, rooms, numRooms)

			PRINT "ID    | ", patientsClone[i].id
			PRINT "Name  | ", patientsClone[i].fname, patientsClone[i].lname
			IF room <> 0 THEN
				PRINT "Room #| ", room
			ELSE 
				PRINT "Room #| None"
			  PRINT "Status| ", patientsClone[i].status
      		ENDIF
		ENDFOR

		PRINT "Save This Report To a File? [Y/N]: "
		READ ch

		IF ch == 'Y' || ch == 'y' THEN
			PRINT "Enter Filename (and extension): "
			READ filename
			
			filePtr = CREATE FILE (filename, for writing)

			IF filePtr <> NULL THEN
				FOR i = 0 TO numPatients DO
					FILE PRINT filePtr, "ID    : ", patientsClone[i].id
					FILE PRINT filePtr, "Name  : ", patientsClone[i].fname, patientsClone[i].lname

					IF (CALL getRoomOfPatient(patientsClone[i].id, rooms, numRooms)) <> 0 THEN
						FILE PRINT filePtr, "Room #: ", CALL getRoomOfPatient(patientsClone[i].id, rooms, numRooms)
					ELSE
						FILE PRINT filePtr, "Room #: None"
					ENDIF
					
					FILE PRINT filePtr, "Status: ", patientsClone[i].status
				ENDFOR

				CLOSE FILE filePtr
				PRINT "FILE CREATED !"
			ELSE
				PRINT "couldn't be accessed.", filename
			ENDIF
		ELSE
			PRINT "Operation Cancelled!"
		ENDIF
	ELSE
		PRINT "NO PATIENTS IN THE SYSTEM!"
    	PRINT "ADD PATIENTS TO VIEW REPORTS"
	ENDIF 
ENDFUNCTION


FUNCTION covid19StatusReport(patients[], numPatients)
	DECLARE none = 0, mild = 0, severe = 0 , critical = 0 , average = 0 , i as INTEGER
	DECLARE filename as STRING
	DECLARE ch, choice as CHARACTER
	DECLARE fileptr as FILE
 
 	PRINT "COVID- 19 STATUS REPORT"
 
	IF numPatients == 0 THEN
		FOR i = 0 < numPatients DO
			IF patients[i].status = "mild" THEN
				mild = mild + 1
			ELSE IF patients[i].status = "severe" THEN
				severe = severe + 1
			ELSE IF patients[i].status = "critical" THEN
				critical = critical + 1
			ELSE IF patients[i].status = "average" THEN
				average = average + 1
			ELSE
				none = none + 1
			ENDIF
		ENDFOR 

		PRINT "NUMBER OF PATIENTS BY COVID STATUS"

		PRINT  "MILD CASES __________" , mild
		PRINT  "SEVERE CASES ________" , severe
		PRINT  "AVERAGE CASES _______" , average
		PRINT  "CRITICAL CASES ______" , critical
		PRINT  "NON COVID CASES _____" , none

		PRINT "Save This Report To a File? [Y/N]: "
		READ choice

		IF choice == 'Y' OR choice == 'y' THEN
			PRINT "  Enter Filename (and extension): "
			READ  filename
			
			filePtr = CREATE FILE (filename, for writing)

			IF filePtr <> NULL THEN
				FILE PRINT filePtr, "MILD CASES ___________", mild
				FILE PRINT filePtr, "SEVERE CASES ________", severe
				FILE PRINT filePtr, "AVERAGE CASES _______", average
				FILE PRINT filePtr, "CRITICAL CASES ______", critical
				FILE PRINT filePtr, "NON COVID CASES _____", none

				CLOSE FILE filePtr
				PRINT  "FILE CREATED !"
			ELSE 
				PRINT " couldn't be accessed.", filename	
			ENDIF
		ELSE 
			PRINT "Operation Cancelled!"
		ENDIF
    ELSE
		PRINT "NO PATIENTS IN THE SYSTEM!"
		PRINT "ADD PATIENTS TO VIEW REPORTS"
	ENDIF
ENDFUNCTION

FUNCTION getRoomOfPatient (patientId, rooms[], numrooms)
	DECLARE roomNum = 0, i, j as INTEGER

	FOR i = 0 TO numrooms DO
		FOR j = 0 TO rooms[i].numPatients DO
			IF rooms[i].patientIds[j] == patientId THEN
				roomNum = rooms[i].roomNum
				EXIT LOOP
			ENDIF
		ENDFOR
	ENDFOR

	RETURN roomNum
ENDFUNCTION

FUNCTION removePatientFromRoom ( patientId, rooms[], numrooms)
	
	DECLARE roomNum = getRoomOfPatient(patientId, rooms, numrooms) as INTEGER
	DECLARE roomIndex, i, j as INTEGER
	DECLARE newPatientIds[MAX_PATIENTS_PER_ROOM] as INTEGER
	
	IF roomNum <> 0 THEN
		FOR roomIndex = 0 TO numrooms DO
			IF rooms[roomIndex].roomNum == roomNum THEN
				j = 0
				
				FOR i = 0 TO rooms[roomIndex].numPatients DO
					IF rooms[roomIndex].patientIds[i] <> patientId THEN
						newPatientIds[j] = rooms[roomIndex].patientIds[i]
						j = j + 1
					ENDIF
				ENDFOR

				rooms[roomIndex].numPatients = rooms[roomIndex].numPatients - 1
				EXIT LOOP
			ENDIF
		ENDFOR
		
		FOR i = 0 TO rooms[roomIndex].numPatients DO
			rooms[roomIndex].patientIds[i] = newPatientIds[i]
		ENDFOR
	ENDIF
ENDFUNCTION

FUNCTION saveChanges( patientsfilename, roomsfilename, patients[], rooms[], numPatients,  numRooms) 
	DECLARE patientFile as FILE
	DECLARE roomFile as FILE
	DECLARE j, i as INTEGER
	DECLARE idList as STRING

	patientFile = OPEN FILE (patientsfilename, for writing)
	roomFile = OPEN FILE (roomsfilename, for writing)

	FOR i = 0 TO numPatients DO
		WRITE patients[i].id,
			patients[i].fname,
			patients[i].lname,
			patients[i].status,
			patients[i].birthYear,
			patients[i].admissionStatus TO FILE patientFile
  	ENDFOR 

	FOR i = 0 TO numRooms DO 
		j = 0
		idList = ""

		REPEAT
			IF rooms[i].patientIds[0] <> 0 THEN 
				idList = idList + rooms[i].patientIds[j] + ","
				j = j + 1
			ELSE
				idList = "none"
				EXIT LOOP
			ENDIF 
		UNTIL rooms[i].patientIds[j] = NULL

		WRITE rooms[i].roomNum,
			rooms[i].respirator,
			rooms[i].numBeds,
			rooms[i].personnel,
			idList TO FILE roomsFile
	ENDFOR

	CLOSE FILE patientFile
	CLOSE FILE roomFile

	PRINT "DATA SUCCESSFULLY SAVED"
ENDFUNCTION