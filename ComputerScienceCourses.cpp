//============================================================================
// Name        : ComputerScienceCourses.cpp
// Author      : Jovi Billiot
// Version     : 1.0
//============================================================================


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "CSVparser.hpp"

using namespace std;

//define a structure to hold bid information
struct Course {
    string courseCode; // unique identifier
    string courseName;
    vector<string> preReqs;

    Course() {}
};


// Internal structure for tree node
struct Node {
    Course course;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};

class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Course course);
    void printSchedule(Node* node);
    void printCourseData(Node* node, string courseCode);


public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void deleteNode(Node* node);
    void Insert(Course course);
    int numPreqCourses(Course course);
    void printSchedule();
    void printCourseData(string courseCode);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr; //root is equal to nullptr
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    deleteNode(root);// recurse from root deleting every node
}

/** Delete node recursively */
void BinarySearchTree::deleteNode(Node *node) {
    if (node){
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }
}

/** Insert node into BST */
void BinarySearchTree::Insert(Course course) {
    /*if root doesn't exist initialize it with current course
     * otherwise add node root and course to BST*/
    if (root == nullptr){
        root = new Node(course);
    }
    else{
        this->addNode(root, course);
    }
}

/** return the number of prerequisites for given course */
int BinarySearchTree::numPreqCourses(Course course){
    int preqCount = 0;
    //check course for any prerequisites and if so count prerequisites
    for (unsigned int i = 0; i < course.preReqs.size(); i++){
        if (course.preReqs.at(i).length() < 0){
            preqCount++;
        }
    }
    return preqCount;
}

/** Pass root to private method */
void BinarySearchTree::printSchedule() {
    this->printSchedule(root);
}

/** search for root and course code and display course information if found */
void BinarySearchTree::printCourseData(string courseCode) {
    this->printCourseData(root, courseCode);
}

/** Add node to BST */
void BinarySearchTree::addNode(Node *node, Course course) {
    // if the current course code is less than the nodes course code
    if (node->course.courseCode.compare(course.courseCode) > 0){
        //if the node to the left is empty, a new node is added to the left
        if (node->left == nullptr){
            node->left = new Node(course);
        }
        // otherwise this node is added to the left
        else{
            this->addNode(node->left, course);
        }
    }
    //else the current course code is greater than the node course code
    else{
        //if the node to the right is empty, a new node is added to the right
        if (node->right == nullptr){
            node->right = new Node(course);
        }
        //otherwise this node is added to the right
        else{
            this->addNode(node->right, course);
        }
    }
}

/** Prints courses in order */
void BinarySearchTree::printSchedule(Node* node) {
    /*as long as node exist, the schedule will print recursively starting
     * with the node pointing to the left then the node pointing to the right.*/
    if (node != nullptr){
        printSchedule(node->left);
        cout << node->course.courseCode << " | " << node->course.courseName << endl;
        printSchedule(node->right);
    }
}

/** Traverse BST to find and display a course's data*/
void BinarySearchTree::printCourseData(Node* currNode, string courseCode) {

    while (currNode != nullptr){
        //check if current node matches and if so displays all course data
        if (currNode->course.courseCode == courseCode){
            cout << currNode->course.courseCode << " | "
            << currNode->course.courseName << endl;
            unsigned int size = numPreqCourses(currNode->course);
            cout << "Prerequisite(s): ";

            // check if course has any prerequisites and if so displays them.
            for (unsigned int i = 0; i < size; i++){
                cout << currNode->course.preReqs.at(i);
                if (i != size - 1){
                    cout << ",  ";
                }
                //if no prerequisites, display message.
                if (i == 0) {
                    cout << "NONE";
                    cout << endl;
                    return;
                }
            }
        }
        //if the course cod eis smaller than the current node's, traverse left.
        else if (currNode->course.courseCode > courseCode){
            currNode = currNode->left;
        }
        //else traverse to the right
        else{
            currNode = currNode->right;
        }
    }
    //display message if the end had been reached without finding course code.
    cout << courseCode << " not found. \n";
}

/** Load courses from file */
bool loadCourses(const string& txtPath, BinarySearchTree* bst){

    cout << "######################################################\n";
    cout << "# Loading CSV file:  " << txtPath << " #\n";
    cout << "######################################################\n\n";

    try{
        //open file
        ifstream coursesFS(txtPath);
        if (coursesFS.is_open()){
            //loop until end of the file
            while (!coursesFS.eof()){
                //vector to hold course data from file
                vector<string> CourseData;
                string courseData;
                //collect data from each line from file
                getline(coursesFS, courseData);

                while (courseData.length() > 0){
                    unsigned int comma = courseData.find(',');
                    if (comma < 100){
                        CourseData.push_back(courseData.substr(0, comma));
                        courseData.erase(0, comma + 1);
                    }
                    else{
                        //add last course after final comma
                        CourseData.push_back(courseData.substr(0, courseData.length()));
                        courseData = "";
                    }
                }
                //load values into a course and add to BST
            Course course;
                course.courseCode = CourseData[0];
                course.courseName = CourseData[1];
                // loop for prerequisites
                for (unsigned int i = 2; i < CourseData.size(); i++){
                    course.preReqs.push_back(CourseData[i]);
                }

                bst->Insert(course); 
            }
            coursesFS.close();
            return true;
        }

    }
    catch (csv::Error& e){
        cerr << e.what() << endl;
    }
    return false;
}

int main(int argc, char* argv[]) {
    //define new binary search tree to hold courses
    BinarySearchTree* CourseTree;
    CourseTree = new BinarySearchTree();
    // user's choice for menu option
    int choice = 0;

    string txtPath, courseCode;

    switch (argc){
        case 2:
            txtPath = argv[1];
            break;
        case 3:
            txtPath = argv[1];
            courseCode = argv[2];
            break;
        default:
            txtPath = "ABCU_Advising_Program_Input.txt";
            break;
    }
    cout << "**********************************\n";
    cout << "*                                *\n";
    cout << "* Welcome to the course planner! *\n";
    cout << "*                                *\n";
    cout << "**********************************\n";

    while (choice != 9) {
        cout << "\n=============================\n";
        cout << "\t\t\tMenu\n";
        cout << "[1] Load Data Structure.\n";
        cout << "[2] Print Course List.\n";
        cout << "[3] Print Course.\n";
        cout << "[9] Exit.\n";
        cout << "=============================\n";
        cout << "\nWhat would you like to do? ";
        cin >> choice;
        cout << "\n";

        if (choice < 1 || choice > 9 || choice >= 4 && choice < 9){
            cout << "Invalid entry: Please try again.\n";
            continue;
        }

        switch (choice){
            // load courses to BST
            case 1:
                bool success;
                success = loadCourses(txtPath, CourseTree);
                if (success) {
                    cout << "Success! Data Structure Loaded!\n";
                }
                else{
                    cout << "Could not load " << txtPath << ".\n";
                }

                break;
            // print the course schedule
            case 2:
                cout << "Computer Science Course List: ";
                CourseTree->printSchedule();
                cout << "\n";
                break;
            //print the data for selected course
            case 3:
                cout << "Which course would you like to know about? ";
                cin >> courseCode;
                CourseTree->printCourseData(courseCode);
                cout << "\n";
                break;
            //end program
            case 9:
                cout << "*******************************************\n";
                cout << "*                                         *\n";
                cout << "* Thank you for using the course planner! *\n";
                cout << "*                                         *\n";
                cout << "*******************************************\n";
                exit(0);
        }
    }
}

