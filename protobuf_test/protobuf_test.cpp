#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"

using namespace std;


string file_name = "address_book.bin";



// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) 
{
    cout << "Enter person ID number: ";
    int id;
    cin >> id;
    person->set_id(id);
    cin.ignore(256, '\n');
    
    cout << "Enter name: ";
    getline(cin, *person->mutable_name());
    
    cout << "Enter email address (blank for none): ";
    string email;
    getline(cin, email);
    if( false == email.empty() )
      person->set_email(email);    

    while (true) 
    {
        cout << "Enter a phone number (or leave blank to finish): ";
        string number;
        getline(cin, number);
        if( number.empty() == true ) 
          break;        
        
        tutorial::Person::PhoneNumber* phone_number = person->add_phones();
        phone_number->set_number(number);
        
        cout << "Is this a mobile, home, or work phone? ";
        string type;
        getline(cin, type);
        if (type == "mobile")         
            phone_number->set_type(tutorial::Person::MOBILE);        
        else if (type == "home")         
            phone_number->set_type(tutorial::Person::HOME);
        else if (type == "work")
            phone_number->set_type(tutorial::Person::WORK);
        else
            cout << "Unknown phone type.  Using default." << endl;        
    }
}




// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int PromptForAddress_example() 
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
   
    tutorial::AddressBook address_book;
    
    {
        // Read the existing address book.
        fstream input( file_name, ios::in | ios::binary);

        if( !input )        
            cout << "error" << endl;        
        else if (!address_book.ParseFromIstream(&input)) 
        {
            cerr << "Failed to parse address book." << endl;
            return -1;
        }
    }
    
    // Add an address.
    PromptForAddress(address_book.add_people());
    
    {
        // Write the new address book back to disk.
        fstream output( file_name, ios::out | ios::trunc | ios::binary);
        if ( !address_book.SerializeToOstream(&output) )
        {
            cerr << "Failed to write address book." << endl;
            return -1;
        }
    }
    
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    
    return 0;
}




// Iterates though all people in the AddressBook and prints info about them.
void ListPeople(const tutorial::AddressBook& address_book) 
{
    for (int i = 0; i < address_book.people_size(); i++) 
    {
        const tutorial::Person& person = address_book.people(i);
        
        cout << "Person ID: " << person.id() << endl;
        cout << "  Name: " << person.name() << endl;
        if (person.has_email())        
            cout << "  E-mail address: " << person.email() << endl;        
        
        for (int j = 0; j < person.phones_size(); j++) 
        {
            const tutorial::Person::PhoneNumber& phone_number = person.phones(j);
            
            switch (phone_number.type()) 
            {
                case tutorial::Person::MOBILE:
                    cout << "  Mobile phone #: ";
                    break;
                case tutorial::Person::HOME:
                    cout << "  Home phone #: ";
                    break;
                case tutorial::Person::WORK:
                    cout << "  Work phone #: ";
                    break;
            }
            cout << phone_number.number() << endl;
        }
    }
}





// Main function:  Reads the entire address book from a file and prints all
//   the information inside.
int ListPeople_example() 
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;   
    
    tutorial::AddressBook address_book;
    
    {
        // Read the existing address book.
        fstream input( file_name, ios::in | ios::binary);
        if (!address_book.ParseFromIstream(&input)) 
        {
            cerr << "Failed to parse address book." << endl;
            return -1;
        }
    }
    
    ListPeople(address_book);
    
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    
    return 0;
}






int main()
{
    PromptForAddress_example();

    ListPeople_example();

    return 0;
}

