K24: SYSTEM PROGRAMMING  
2i ergasia - earino e3amino


Eleftheria Ellina
A.M. : 1115201800228


1.  Arxeia pou paradidonte

Mesa ston fakelo .tar.gz iparxi enas fakelos HW2 me 16 arxeia, 14 ek ton opion apoteloun ta .c kai .h files, ena makefile kai afto to readme(se txt), kathos kai o fakelos input_dir
pou dimiourgithike apo to bash. Episis periexete o fakelos tou bash script me to script, to inputFile me 500 records kai o fakelos input_dir pou dimiourgithike apo to bash me to 
proanaferomeno inputFile.


2. Ektelesi programmatos kai create_infiles.sh

Compile with make and excecute with command below:
./travelmonitor -m <numMonitors> -b <bufferSize> -s <sizeOfBloom> -i input_dir, numMonitor, bufferSize kai sizeOfBloom pou epithimoume.

Dokimastike sta linux mixanimata tis sxolis.  Mia endiktiki entoli einai:  
./travelmonitor -m 5 -b 15 -s 800000 -i input_dir

Gia tin ektelesi tou script trexoume ./create_infiles.sh inputFile input_dir numFilesPerDirectory, me ta inputs pou theloume.


3. Sxediastikes Epiloges

Exoun ilopioithei ola ta zitoumena.
 
I sosti litourgia ton signals dokimastike me:
- kill(pid[0], SIGKILL) gia to SIGCHLD ston travelmonitor:
	edo na anafero oti kata tin klisi aftou tou signal allazi to sigclhd_flag apo ton handler kai kata ton elegxo tou flag aftou dimiourgite
	neo paidi to opoio perni oles tis domes kai ta dedomena (countries, records, bloomfilters)tou pediou pou pethane oste na to antikatastisi 
	pliros gia tin omali sinexia tou programmatos

- kill(getpid(), SIGINT) i kill(getpid(), SIGQUIT) gia ta SIGQUIT KAI SIGINT ston travelmonitor opou me tin getpid() pire to pid tou patera:
- kill(getpid(), SIGINT) i kill(getpid(), SIGQUIT) gia ta SIGQUIT KAI SIGINT ston monitor opou me tin getpid() pire to pid tou pediou:
	edo na anafero oti kata tin klisi afton ton signals to pedi allazei to sig_flag ston handler kai me elegxo tou flag dimiourgi ena 
	logfile me tis xores kai ta statistika tou

ta kill afta exoun diagrafei apo to programma oste na e3etasoun ta signals opos theloun i e3etastes,
kai telos:
- kill(pid[i], SIGUSR1) gia to SIGUSR1 ston travelmonitor gia tin litourgia tou addVaccinationRecords

Exoun xrisimopioithei domes aftousies apo to Project1, autes ton Records, BloomFilters (domi bloom sto pedi) opos kai ta Skiplists. Episis iparxoun i domes country, 
countryTO (gia ti sillogi stoixeion apo to travelRequest kai ektiposi tous sto travelStats), virus, stats, txt (gia tin ektiposi ton xoron sta logfiles ton monitor),
bloomfilter (defteri domi bloom ston patera).

 
4. Perigrafi programmatos

Arxika ginete dilosi ton struct ton signal gia tin sosti li4i kai antimetopisi ton zitoumenon signals. Stin sinexia gia kathe monitor dimiourgounte ena read kai ena write pipe meso
tis create_pipes. Sti sinexia kalite i fork oste na dimiourgisi ta pedia kai kalei me execlp tin diadikasia monitor me orismata ta pipe kathe pediou. sti sinexia anigoun ta pipes se patera
kai pedi kai arxizi i epikinonia kai antallagi dedomenon meso pipes. O pateras pernai sta monitor to bufferSize kai ta paths pou antistoixoun se kathe pedi, sti sinexia tous pernaei to
sizeOfBloom.
Sto meta3i to kathe pedi anigei ta arxeia ton directories pou tou exoun anatethei apo ton patera kai dimiourgoun tin domi records pou filaei ta records pou anikoun se kathe monitor
kai txt i opia filaei sto pedi tis xores pou antistixoun se kathe monitor. Sti sinexia afou diavazi to bloomsize dimiourgi ti domi bloom gia kathe virus mesa sto monitor kai 
stin sinexia stelni ta bloom piso ston patera. O pateras ta diavazi kai ta apothikevi stin domi bloomFilter gia elegxous pou tha proki4oun stin sinexia. 
Stin sinexia o monitor dimiourgi ti domi virus me ola ta virus pou exei to monitor mesa to opoio tha xrisimopieithi gia elegxous stin sinexia.

Arxizi meta i diadikasia ton aitimaton apo ton xristi. O xristis dini tin entoli ston patera kai aftos epe3ergazete kathe entoli analoga, otan xreiastei na epikinonisi me to pedi tote tou stelni
tin entoli kai to pedi xirizete me ton diko tou tropo tin entoli. Genika iparxi antalagi dedomenon meta3i patera kai pediou analoga me tin entoli. Afto ginete kirios sta aitimata travelRequest,
addVaccinationRecords kai searchVaccinationStatus. I ilopiisi olon ton aitimaton ginete me ti sigkrisi ton kommation tou command pu tha dosi o xristis kai ton domon tou patera kai tou pediou.
meta apo tous analogous elegxous dinonte i analoges apantisis.

travelRequest:
arxika 4axnei sta bloomFilter na dei an o citizenID einai maybe vaccinated ki an einai tote stelni tin entoli sto pedi to opoio vriski apo ta skiplist an einai ontos vaccinated kai entos ton 
6 minon kai stelni apantisi ston patera o opios tin tiponi. Kata to request afto af3anonte i metrites ton total accepted kai rejected counters toso tou patera oso kai tou pediou gia na tipothoun
sta logfiles tous. Episis ginete se kathe request update i domi ton statistics pou tha xrisimopiithi gia to travelStats.
Se periptosi pou den iparxi kamia pliroforia gia to citizenID tote ipothetoume oti den einai vaccinated gia ton io pou dothike kai ginete af3isi ton rejected aitimaton.

travelStats:
vriski to virus sti lista ton stats kai dinei ta statistika gia tis imerominies pou dothikan apo ton xristi. kathe komvos exei virus kai xora me mia accepted i mia rejected request. Pernaei pano
apo oli ti lista kai otan vri idio virus kai xora tote elegxei tis imerominies pou dothikan kai prostheti ta statistika kathe komvous se sinolikous metrites gia ta total accepted kai rejected
ta opia sto telos tiponei.

addVaccinationRecords:
vriski ti xora kai se pio monitor einai. stelni sima SIGUSR1 sto sigkekrimeno monitor to opio anixnevi o sigkekrimenos monitor kai allazi to sigusr1_flag ston monitor. otan to flag afto allazi ginete
i sarosi gia kenourgio arxeio kai an vrethei tote ananeononte ta bloom kai ta virus lists ston monitor kai stin sinexia stelnonte ta updated bloom ston patera o opios otna ta lavi diagrafei
apo ta palia bloomFilter ta nodes tou monitor kai ta antikathista me afta pou tou stelnonte. 
simiosi: ta arxeia pou prosthetonte prepei na periexoun records me allagi grammis sto telos kathe record

searchVaccinationStatus:
o pateras stelni tin entoli amesos ston monitor o opios entopizi to citizenID kai stelni ola ta stoixeia tou ston patera mazi kai me ta virus kai to an einai vaccinated i oxi, o pateras ta diavazei 
kai ta tiponei.

exit:
eite dothi exit eite dothei SIGINT/SIGQUIT ston patera tote stelnei SIGKILL signal sta pedia tou kai dimiourgi ena logfile me tis xores tou travelmonitor kai ta statistika tou kai kalei break
to opoio stamatei to while(1) me apotelesma na proxora o parent se apeleftherosi tis opoiasdipote desmevmenis mnimis. 



