#include "inputs.h"
#include "constants.h"
#include "structs.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "RS232.h" // may need to change path 

using namespace std;

// ******************* bluetooth place holder *******************
string ask_for_name() {
    string name; 
    cout << "Please type your name: "; // Type a number and press enter
    cin >> name; // Get user input from the keyboard
    cout << "Your name is: " << name << endl; // Display the input value

    return name;
}

setupvalues ask_for_setup(int player_num) {
    int x;
    int y;
    int size;
    int orientation;
    int device_num;

    cout << "Player " << player_num << endl;
    cout << "What x position would you like to place your ship: ";
    cin >> x;
    cout << "What y position would you like to place your ship: ";
    cin >> y;
    cout << "What size is the ship: ";
    cin >> size;
    cout << "What orientation is the ship (1 for vertical and 2 for horizntaal): ";
    cin >> orientation;

    device_num = player_num;

    return setupvalues(x, y, size, orientation, device_num);
}

shootvalues ask_for_shoot(int player_num) {
    int x;
    int y;
    int device_num;

    cout << "Player " << player_num << endl;
    cout << "What x position would you like to shoot: ";
    cin >> x;
    cout << "What y position would you liek to shoot: ";
    cin >> y;

    device_num = player_num;

    return shootvalues(x, y, device_num);

}

// ******************* bluetooth place holder *******************
// taking in a list of inputs 
/*
Format: 
“placement 2 3 3 1 \n7 2 2 2 \n5 5 4 2~”
*/
void get_placement_message_BT(list<setupvalues> *list_setupval, string inputstring, int device_num) {
    //string input = inputstring;
    char receive_char[256];
    if (device_num == 1) {
        BT_receive_0(receive_char);
    }
    else {
        BT_receive_1(receive_char);
    }

    string input = string(receive_char);
    stringstream ss;

    ss << input;

    // dealing the keyword placement in front of the message
    string keyword;
    ss >> keyword;
    
    for (int i = 0; i < NUM_OF_SHIPS; i++) {

        int x,y,size, orientation;
        ss >> x >> y >> size >> orientation;
        (*list_setupval).push_back(setupvalues(x, y, size, orientation, device_num));
        // std::cout << "x: " << x << '\n';
        // std::cout << "y: " << y << '\n';
        // std::cout << "size: " << size << '\n';
        // std::cout << "orientation: " << orientation << '\n';
        // std::cout << ss.str() << endl; // converting string stream to string 
    }
}

/*
Format:
“shoot xCoordinate yCoordinate\n\n”
*/
shootvalues get_shoot_message_BT(int device_num) {
    int x,y;

    char receive_char[256];
    if (device_num == 1) {
        BT_receive_0(receive_char);
    }
    else {
        BT_receive_1(receive_char);
    }

    string input = string(receive_char);
    stringstream ss;

    ss << input;

    // dealing the keyword placement in front of the message
    string keyword;
    ss >> keyword;

    ss >> x >> y;

    return shootvalues(x, y, device_num);
}

/*
Format:
“game start isTurn~”
isTurn is 1 if the current player is attacking first, 0 otherwise
*/
void send_game_start_status_BT(bool start1) {
    string message_to_1 = "game start ";
    string message_to_2 = "game start ";

    
    message_to_1 += to_string(start1);
    message_to_2 += to_string(!start1);

    message_to_1 += "~";
    message_to_2 += "~";

    int n = message_to_1.length();
 
    // declaring character array with space for termination character 
    char message_to_1_char[n + 1];
    char message_to_2_char[n + 1];
 
    // copying the contents of the string to char array
    strcpy(message_to_1_char, message_to_1.c_str());
    strcpy(message_to_2_char, message_to_2.c_str());

    BT_send_0(message_to_1_char);
    BT_send_1(message_to_2_char);

}

/*
Format: (the result for attacking)
“result xCoordinate yCoordinate hitStatus gameStatus~”
*/
void send_result_message_BT(int device_num, int x, int y, int hitstatus, int gamestatus) {
    string message = "result ";

    message += to_string(x);
    message += " ";
    message += to_string(y);
    message += " ";
    message += to_string(hitstatus);
    message += " ";
    message += to_string(gamestatus);

    message += "~";

    int n = message.length();
 
    // declaring character array with space for termination character 
    char message_char[n + 1];
 
    // copying the contents of the string to char array
    strcpy(message_char, message.c_str());

    if (device_num == 1) {
        BT_send_0(message_char);
    }
    else {
        BT_send_1(message_char);
    }


}

/*
Format: (the result of being attacked)
“targeted xCoordinate yCoordinate gameStatus shipStatus (destroyedShipXCoordinate destroyedShipYCoordinate shipLength shipOrientation)~”
*/
void send_targeted_message_BT(int device_num, int x, int y, int gamestatus, int shipstatus) {
    string message = "targeted ";

    message += to_string(x);
    message += " ";
    message += to_string(y);
    message += " ";
    message += to_string(gamestatus);
    message += " ";
    message += to_string(shipstatus);

    message += "~";

    int n = message.length();
 
    // declaring character array with space for termination character 
    char message_char[n + 1];
 
    // copying the contents of the string to char array
    strcpy(message_char, message.c_str());

    if (device_num == 1) {
        BT_send_0(message_char);
    }
    else {
        BT_send_1(message_char);
    }

}

/*
Format: (the result of being attacked)
“targeted xCoordinate yCoordinate gameStatus shipStatus (destroyedShipXCoordinate destroyedShipYCoordinate shipLength shipOrientation)~”
*/
void send_targeted_message_BT(int device_num, int x, int y, int gamestatus, int shipstatus, int destroyed_start_x, int destroyed_start_y, int length, int orientation) {
    string message = "targeted ";

    message += to_string(x);
    message += " ";
    message += to_string(y);
    message += " ";
    message += to_string(gamestatus);
    message += " ";
    message += to_string(shipstatus);
    message += " ";
    message += to_string(destroyed_start_x);
    message += " ";
    message += to_string(destroyed_start_y);
    message += " ";
    message += to_string(length);
    message += " ";
    message += to_string(orientation);

    message += "~";

    int n = message.length();
 
    // declaring character array with space for termination character 
    char message_char[n + 1];
 
    // copying the contents of the string to char array
    strcpy(message_char, message.c_str());

    if (device_num == 1) {
        BT_send_0(message_char);
    }
    else {
        BT_send_1(message_char);
    }

}


/*
Format:
“create playerEmail numPlayers~”
playerEmail is the email of the player
numPlayers is 1 or 2, 1 for vs AI, 2 for multiplayer
*/
createmessage get_create_message_BT(int device_num) {
    char receive_char[256];

    if (device_num == 1) {
        BT_receive_0(receive_char);
    }
    else {
        BT_receive_1(receive_char);
    }

    string input = string(receive_char);
    stringstream ss;

    ss << input;

    // dealing the keyword placement in front of the message
    string keyword;
    ss >> keyword;

    string email;
    int num_players;

    ss >> email >> num_players;

    return createmessage(num_players, email);
    
}

/*
Format:
“create numPlayers status~”
numPlayers is 1 or 2, 1 for vs AI, 2 for multiplayer
status is “1” or “0”
“0” if a game is already in progress
Else “1” if multiplayer lobby created successfully
*/
void send_create_message_response_BT(int device_num, int num_players, int status) {
    string message = "create ";

    message += to_string(num_players);
    message += " ";
    message += to_string(status);

    message += "~";

    int n = message.length();
 
    // declaring character array with space for termination character 
    char message_char[n + 1];
 
    // copying the contents of the string to char array
    strcpy(message_char, message.c_str());

    if (device_num == 1) {
        BT_send_0(message_char);
    }
    else {
        BT_send_1(message_char);
    }
}


/*
Format:
“ready~”
Only sent for multiplayer lobbies
Sent when the multiplayer lobby is full and the players may begin placement
*/

void send_ready_messaeg_BT() {
    BT_send_0("ready~");
    BT_send_1("ready~");
}
