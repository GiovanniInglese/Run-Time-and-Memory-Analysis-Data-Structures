#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>  // used only for validation; BST is the chosen DS
#include <algorithm>

using namespace std;

//struct for courses
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

//struct for nodes
struct Node {
    Course course;
    Node* left = nullptr;
    Node* right = nullptr;

    explicit Node(const Course& c) : course(c) {}
};

//Binary search tree for courses, insert, search, inorder functions
class CourseBST {
public:
    CourseBST() = default;
    
    
    ~CourseBST() { 
        destroy(root); 
    }

    // Insert a course into the BST (by courseNumber)
    void insert(const Course& c) {
        if (!root) { 
            root = new Node(c); return; 
        }
        addNode(root, c);
    }

    // Find a course by number; returns nullptr if not found
    const Course* search(const string& number) const {
        return searchRec(root, number);
    }

    // Print all courses in alphanumeric order (ascending by courseNumber)
    void printInOrder() const {
        inOrder(root);
    }

    // Clear the tree used when re-loading
    void clear() {
        destroy(root);
        root = nullptr;
    }

private:
    Node* root = nullptr;

    // Recursive insert helper
    static void addNode(Node* node, const Course& c) {
        if (c.courseNumber < node->course.courseNumber) {
            if (!node->left) node->left = new Node(c);
            else addNode(node->left, c);
        }
        else {
            if (!node->right) node->right = new Node(c);
            else addNode(node->right, c);
        }
    }

    // Recursive search helper
    static const Course* searchRec(Node* node, const string& number) {
        if (!node) return nullptr;
        if (number == node->course.courseNumber) 
            return &node->course;
        if (number < node->course.courseNumber)  
            return searchRec(node->left, number);
        return searchRec(node->right, number);
    }

    // In-order traversal 
    static void inOrder(Node* node) {
        if (!node) return;
        inOrder(node->left);
        cout << node->course.courseNumber << ": " << node->course.courseTitle << '\n';
        inOrder(node->right);
    }

    // Recursively free nodes
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
};


static vector<string> splitCSV(const string& line) {
  //splits the csv into tokens for number, title, and prereqs
 //seperates tokens by comma and extracts them from each line and append them to the out vector
    vector<string> out;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ',')) out.push_back((token));
    return out;
}



//Load courses from file and prompts file name
static bool loadCoursesFromFile(CourseBST& tree) {

    
    cout << "Enter file path (no quotations marks)" << endl;
    string path;
    getline(cin, path);

    


        
    //checks if file is empty
    if (path.empty()) {
        cerr << "Error: No filename provided.\n";
        return false;
    }
    //opens file
    ifstream in(path);
    if (!in) {
        cerr << "Error: Couldn't open file remove quotations '" << path << "'.\n";
        return false;
    }
    //creates vector to hold all courses
    vector<Course> allCourses;
    string line;
    size_t lineNo = 0;

    while (getline(in, line)) {
        lineNo++;
        string raw = (line);
        if (raw.empty()) continue; // skip blanks

        vector<string> tokens = splitCSV(raw);

        // Need at least courseNumber and courseTitle
        if (tokens.size() < 2) {
            cerr << "Error (line " << lineNo << "): Missing course number or title.\n";
            continue;
        }
        // takes course number and course title and assigns then to a token split by the csv split 
        Course c;
        c.courseNumber = tokens[0];
        c.courseTitle = tokens[1];

        // Remaining tokens are prerequisites (if any)
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) c.prerequisites.push_back(tokens[i]);
        }
        //appends all courses to all courses with prereqs
        allCourses.push_back(c);
    }
    in.close();

    if (allCourses.empty()) {
        cerr << "No valid course rows found.\n";
        return false;
    }

    // Build a set of all course numbers for prerequisite existence check
    unordered_set<string> existing;
    existing.reserve(allCourses.size() * 2);
    for (const auto& c : allCourses) existing.insert(c.courseNumber);

    // Validate prerequisites exist in the file
    for (const auto& c : allCourses) {
        for (const auto& p : c.prerequisites) {
            if (!p.empty() && existing.find(p) == existing.end()) {
                cerr << "Warning: prerequisite '" << p
                    << "' referenced by " << c.courseNumber
                    << " not found in file.\n";
            }
        }
    }

    // Clear the BST and insert all courses
    tree.clear();
    for (const auto& c : allCourses) {
        tree.insert(c);
    }

    cout << "Loaded " << allCourses.size() << " course(s) from '" << path << "'.\n";
    return true;
}

static std::string toUpper(std::string s) {
    for (char& ch : s)
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}


//Print a single course info from key course number
static void printCourseInfo(const CourseBST& tree) {


    
    cout << "Enter course number: ";
    string key;
    getline(cin, key);
    key = toUpper(key);

    if (key.empty()) {
        cerr << "Error: No course number entered.\n";
        return;
    }
    //assigns pointer c with specific course based on search key
    const Course* c = tree.search(key);
    if (!c) {
        cout << "Course not found.\n";
        return;
    }
    //prints the course number and title
    cout << c->courseNumber << ": " << c->courseTitle << '\n';

    if (c->prerequisites.empty()) {
        cout << "No prerequisites\n";
        return;
    }
    //prints prereqs checks if a course has a prereq and prints it
    cout << "Prerequisites:\n";
    for (const auto& p : c->prerequisites) {
        const Course* pc = tree.search(p); // look up each prereq in the BST
        if (pc) {
            cout << "  " << pc->courseNumber << ": " << pc->courseTitle << '\n';
        }
        else {
            // In case the prereq was missing or misspelled
            cout << "  " << p << " (title not found)\n";
        }
    }
}

//Prints all courses in order
static void printCourseList(const CourseBST& tree) {
    cout << "\n=== Course List (Alphanumeric) ===\n";
    tree.printInOrder();
}
//Main menu loop
int main() {
    CourseBST courseTree;      // Chosen data structure that holds Course objects
    bool dataLoaded = false;   // gate other options until data is loaded

    for (;;) {
        cout << "\n===== ABCU Course Planner =====\n"
            << " 1. Load Data Structure (from file)\n"
            << " 2. Print Course List (Alphanumeric)\n"
            << " 3. Print Course Information (Title & Prereqs)\n"
            << " 9. Exit\n"
            << "Select an option: ";


        //menu loop for choice options with error handling

        string rawChoice;
        if (!getline(cin, rawChoice)) break;   // handle EOF
        int choice = 0;
        { stringstream ss(rawChoice); ss >> choice; }

        if (choice == 1) {
            dataLoaded = loadCoursesFromFile(courseTree);
        }
        else if (choice == 2) {
            if (!dataLoaded) {
                cerr << "Please load data first (Option 1).\n";
            }
            else {
                printCourseList(courseTree);
            }
        }
        else if (choice == 3) {
            if (!dataLoaded) {
                cerr << "Please load data first (Option 1).\n";
            }
            else {
                printCourseInfo(courseTree);
            }
        }
        else if (choice == 9) {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cerr << "Invalid option. Please choose 1, 2, 3, or 9.\n";
        }
    }
    return 0;
}

 




