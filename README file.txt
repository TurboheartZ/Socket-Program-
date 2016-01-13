a. Full Name:  Xinyi Zhang

b. USC ID:  9849507799

c. What I have done: I programmed a healthcenterserver program, 2 doctor programs as well as 2 patient programs as shown below:
   
d. healthcenterserver.c : In phase1, the healthcenterserver connects patients with TCP connections and checks their credentials,
                          if matches, the healthcenterserver will take the patients program into phase2.
                          In phase2, the healthcenterserver will send the available time slots to the patients and check the availability
                          after patients make their choice.If the appointment time slot is available, it will send the port number of
                          the assigned docotor to the patient.

   docotor1.c : doctor1 will be listening on its UDP port for the request of patients in phase3 and once the patients send their insurance
                plan to the doctor over UDP connections, the doctor will check his copay policy file doc1.txt and send back a message containing
                the estimated price of the appointment.

   docotor2.c : doctor2 has the same duty as docotor1 does.

   patient1.c : In phase1, the patient will connect healthcenterserver over TCP connections. It will load its username and password and then
                send them to the healthcenterserver and let the server to check the patient's credentials. If this credential check fails, the
                patient program will be terminated. If the check successes, the program will enter pahse2.
                In phase2, the patient will receive the appointment time slot from the healthcenterserver and be asked to choose.If the time
                is available, the patient will receive a message containing th port number of the assigned doctor.
                In phase3, the patient will build up a UDP connection with assigned doctor, it will loads its insurance plan file and send it 
                to the doctor. Then the patient will receive the estimated price of the appointment.

   patient2.c : patient2 has the same funcitons as patient1 does.    
   
e. To run these programs, I hope you can run healthcenterserver first and the doctor programs and then the patients programs. 

f. All the messages exchanged between these programs are in char string format.

g. Idiosyncrasy: None.

h. I have used some codes from Beej's guide book, they are listed below:
   
   1. get_in_addr function        from    Beej's guide book
   
   2. sigchld_handler function    from    Beej's guide book