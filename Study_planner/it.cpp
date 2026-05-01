#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// -------------------- STRUCTS --------------------
struct Officer {
    string username;
    string password;
    Officer(const string& u, const string& p) : username(u), password(p) {}
};

struct Subject {
    string username; // owner
    string name;
    int importance;
    int timeRequired;
    int deadline;
};

// -------------------- FILE PATHS --------------------
const string USER_FILE = "D:\\Local Disk E\\codeblocks C++\\Example folder\\studentpassword.txt";
const string SUBJECT_FILE = "D:\\Local Disk E\\codeblocks C++\\Example folder\\subjects.txt";

// -------------------- FUNCTION PROTOTYPES --------------------
void displayMenu();
void addSubject(vector<Subject>& subjects, const string& username);
void viewSubjects(const vector<Subject>& subjects);
void updateSubject(vector<Subject>& subjects, const string& username);
void deleteSubject(vector<Subject>& subjects, const string& username);
void knapsackScheduler(const vector<Subject>& subjects, int totalTime);
void greedyScheduler(const vector<Subject>& subjects, int totalTime);
void jobSequencingScheduler(const vector<Subject>& subjects, int totalDays);
void compareAlgorithms(const vector<Subject>& subjects, int totalTime, int totalDays);
vector<Officer> readLoginFile();
bool createAccount();
bool login(string &username);
void saveOfficer(const Officer& officer);
void saveSubjectToFile(const Subject& subject);
vector<Subject> readSubjectsForUser(const string& username);
void overwriteAllSubjects(const vector<Subject>& allSubjects);

// -------------------- MAIN --------------------
int main() {
    vector<Subject> subjects;
    int totalTime, totalDays, choice;
    string username;

    cout << "======================================\n";
    cout << "   Welcome to Smart Study Planner!    \n";
    cout << "======================================\n";

    // Login/Create Account
    int option;
    cout << "1. Login\n2. Create New Account\nEnter choice: ";
    cin >> option;

    bool loggedIn = false;
    while(!loggedIn) {
        if(option == 1) {
            loggedIn = login(username);
        } else if(option == 2) {
            if(createAccount()) {
                loggedIn = login(username);
            } else break;
        }
        if(!loggedIn) {
            cout << "Try again.\nUsername or password incorrect.\n";
        }
    }

    if(!loggedIn) {
        cout << "Exiting program.\n";
        return 0;
    }

    cout << "\nWelcome, " << username << "!\n";

    // Load user's subjects
    subjects = readSubjectsForUser(username);

    cout << "Enter total available study time (hours): ";
    cin >> totalTime;
    cout << "Enter total available days: ";
    cin >> totalDays;

    cout << "\nPlanner ready!\n";

    do {
        displayMenu();
        cin >> choice;

        switch(choice) {
            case 1: addSubject(subjects, username); break;
            case 2: viewSubjects(subjects); break;
            case 3: knapsackScheduler(subjects, totalTime); break;
            case 4: greedyScheduler(subjects, totalTime); break;
            case 5: jobSequencingScheduler(subjects, totalDays); break;
            case 6: compareAlgorithms(subjects, totalTime, totalDays); break;
            case 7: updateSubject(subjects, username); break;
            case 8: deleteSubject(subjects, username); break;
            case 9: overwriteAllSubjects(subjects); cout << "Subjects saved successfully!\n"; break;
            case 10: cout << "Thank you for using Smart Study Planner!\n"; break;
            default: cout << "Invalid choice! Please try again.\n";
        }
    } while(choice != 10);

    return 0;
}

// -------------------- MENU --------------------
void displayMenu() {
    cout << "\n MAIN MENU \n";
    cout << "1. Add New Subject\n";
    cout << "2. View All Subjects\n";
    cout << "3. Generate Schedule (0/1 Knapsack - DP)\n";
    cout << "4. Generate Schedule (Greedy Algorithm)\n";
    cout << "5. Generate Schedule (Job Sequencing)\n";
    cout << "6. Compare All Algorithms\n";
    cout << "7. Update Subject Time\n";
    cout << "8. Delete Subject\n";
    cout << "9. Save Subjects\n";
    cout << "10. Exit\n";
    cout << "Enter your choice (1-10): ";
}

// -------------------- ADD SUBJECT (WITH DUPLICATE CHECK) --------------------
void addSubject(vector<Subject>& subjects, const string& username) {
    Subject newSubject;
    newSubject.username = username;

    cin.ignore();
    cout << "\n--- Add New Subject ---\n";
    cout << "Subject Name: ";
    getline(cin, newSubject.name);

    // Check if subject already exists
    for(auto& s : subjects) {
        if(s.name == newSubject.name) {
            char choice;
            cout << "Subject \"" << s.name << "\" already exists. Do you want to update it? (y/n): ";
            cin >> choice;
            if(choice == 'y' || choice == 'Y') {
                int newTime, newImportance;
                do {
                    cout << "Enter new time required (>0): ";
                    cin >> newTime;
                    if(newTime <= 0)
                        cout << "Error: Time must be greater than 0.\n";
                } while(newTime <= 0);
                s.timeRequired = newTime;

                do {
                    cout << "Enter new importance (1-100): ";
                    cin >> newImportance;
                    if(newImportance < 1 || newImportance > 100)
                        cout << "Error: Importance must be between 1 and 100.\n";
                } while(newImportance < 1 || newImportance > 100);
                s.importance = newImportance;

                cout << "Subject updated successfully.\n";
                overwriteAllSubjects(subjects); // update file
                return;
            } else {
                cout << "Add subject canceled.\n";
                return;
            }
        }
    }

    // Importance validation
    do {
        cout << "Importance (1-100): ";
        cin >> newSubject.importance;
        if(newSubject.importance < 1 || newSubject.importance > 100)
            cout << "Error: Importance must be between 1 and 100.\n";
    } while(newSubject.importance < 1 || newSubject.importance > 100);

    // Time validation
    do {
        cout << "Time Required (hours, >0): ";
        cin >> newSubject.timeRequired;
        if(newSubject.timeRequired <= 0)
            cout << "Error: Time must be greater than 0.\n";
    } while(newSubject.timeRequired <= 0);

    cout << "Deadline (day): ";
    cin >> newSubject.deadline;

    subjects.push_back(newSubject);
    saveSubjectToFile(newSubject);
    cout << "Subject added successfully!\n";
}

// -------------------- VIEW SUBJECTS --------------------
void viewSubjects(const vector<Subject>& subjects) {
    if(subjects.empty()) {
        cout << "No subjects available!\n";
        return;
    }

    cout << "\n--- All Subjects ---\n";
    cout << setw(15) << "Subject" << setw(15) << "Importance"
         << setw(15) << "Time Needed" << setw(12) << "Deadline\n";
    cout << string(57, '-') << endl;

    for(const auto& subject : subjects) {
        cout << setw(15) << subject.name
             << setw(15) << subject.importance
             << setw(15) << subject.timeRequired
             << setw(12) << subject.deadline << endl;
    }
}

// -------------------- UPDATE SUBJECT --------------------
void updateSubject(vector<Subject>& subjects, const string& username) {
    if(subjects.empty()) {
        cout << "No subjects to update!\n";
        return;
    }

    string name;
    cin.ignore();
    cout << "Enter the subject name to update: ";
    getline(cin, name);

    bool found = false;
    for(auto& s : subjects) {
        if(s.name == name) {
            found = true;
            cout << "Current time required: " << s.timeRequired << " hours, Importance: " << s.importance << "\n";
            int newTime, newImportance;
            do {
                cout << "Enter new time required (>0): ";
                cin >> newTime;
                if(newTime <= 0) cout << "Error: Time must be greater than 0.\n";
            } while(newTime <= 0);
            s.timeRequired = newTime;

            do {
                cout << "Enter new importance (1-100): ";
                cin >> newImportance;
                if(newImportance < 1 || newImportance > 100) cout << "Error: Importance must be 1-100.\n";
            } while(newImportance < 1 || newImportance > 100);
            s.importance = newImportance;

            cout << "Subject updated successfully.\n";
            overwriteAllSubjects(subjects);
            break;
        }
    }
    if(!found) cout << "Subject not found.\n";
}

// -------------------- DELETE SUBJECT --------------------
void deleteSubject(vector<Subject>& subjects, const string& username) {
    if(subjects.empty()) {
        cout << "No subjects to delete!\n";
        return;
    }

    string name;
    cin.ignore();
    cout << "Enter the subject name to delete: ";
    getline(cin, name);

    bool found = false;
    for(auto it = subjects.begin(); it != subjects.end(); ++it) {
        if(it->name == name) {
            found = true;
            subjects.erase(it);
            overwriteAllSubjects(subjects);
            cout << "Subject deleted successfully.\n";
            break;
        }
    }
    if(!found) cout << "Subject not found.\n";
}

// -------------------- KNAPSACK --------------------
void knapsackScheduler(const vector<Subject>& subjects, int totalTime) {
    if(subjects.empty()) {
        cout << "No subjects to schedule!\n";
        return;
    }
    int n = subjects.size();
    vector<vector<int>> dp(n+1, vector<int>(totalTime+1,0));
    for(int i=1;i<=n;i++){
        for(int w=0;w<=totalTime;w++){
            if(subjects[i-1].timeRequired <= w)
                dp[i][w] = max(dp[i-1][w], subjects[i-1].importance + dp[i-1][w - subjects[i-1].timeRequired]);
            else dp[i][w] = dp[i-1][w];
        }
    }

    vector<Subject> selected;
    int w=totalTime;
    for(int i=n;i>0 && w>0;i--){
        if(dp[i][w] != dp[i-1][w]){
            selected.push_back(subjects[i-1]);
            w -= subjects[i-1].timeRequired;
        }
    }

    cout << "\n0/1 KNAPSACK SCHEDULE (DP)\n";
    cout << "Max Importance: " << dp[n][totalTime] << "\n";
    cout << "Selected Subjects:\n";
    int totalTimeUsed=0;
    for(const auto& s:selected){
        cout << s.name << " (Time: " << s.timeRequired << ", Importance: " << s.importance << ")\n";
        totalTimeUsed += s.timeRequired;
    }
    cout << "Total Time Used: " << totalTimeUsed << "/" << totalTime << "\n";
}

// -------------------- GREEDY --------------------
void greedyScheduler(const vector<Subject>& subjects, int totalTime) {
    if(subjects.empty()){ cout << "No subjects to schedule!\n"; return; }

    vector<pair<double,Subject>> ratioSubjects;
    for(const auto& s:subjects) ratioSubjects.push_back({(double)s.importance/s.timeRequired,s});
    sort(ratioSubjects.begin(), ratioSubjects.end(), [](auto &a, auto &b){return a.first>b.first;});

    vector<Subject> selected;
    int currentTime=0, totalImportance=0;
    for(const auto& [r,s]:ratioSubjects){
        if(currentTime + s.timeRequired <= totalTime){
            selected.push_back(s);
            currentTime += s.timeRequired;
            totalImportance += s.importance;
        }
    }

    cout << "\nGREEDY SCHEDULE\n";
    cout << "Total Importance: " << totalImportance << "\n";
    for(const auto& s:selected){
        double r = (double)s.importance/s.timeRequired;
        cout << s.name << " (Time: " << s.timeRequired << ", Importance: " << s.importance
             << ", Ratio: " << fixed << setprecision(2) << r << ")\n";
    }
    cout << "Total Time Used: " << currentTime << "/" << totalTime << "\n";
}

// -------------------- JOB SEQUENCING --------------------
void jobSequencingScheduler(const vector<Subject>& subjects, int totalDays) {
    if(subjects.empty()){ cout << "No subjects to schedule!\n"; return; }

    vector<Subject> sortedSubjects = subjects;
    sort(sortedSubjects.begin(), sortedSubjects.end(), [](const Subject &a,const Subject &b){return a.deadline < b.deadline;});
    vector<bool> slot(totalDays,false);
    vector<Subject> result(totalDays);
    int totalImportance=0;

    for(const auto& s:sortedSubjects){
        for(int j=min(totalDays,s.deadline)-1;j>=0;j--){
            if(!slot[j]){
                slot[j]=true;
                result[j]=s;
                totalImportance+=s.importance;
                break;
            }
        }
    }

    cout << "\nJOB SEQUENCING SCHEDULE\n";
    cout << "Total Importance: " << totalImportance << "\n";
    for(int i=0;i<totalDays;i++){
        if(slot[i])
            cout << "Day " << i+1 << ": " << result[i].name << " (Time: "
                 << result[i].timeRequired << ", Importance: " << result[i].importance
                 << ", Deadline: Day " << result[i].deadline << ")\n";
        else cout << "Day " << i+1 << ": Free/No subject\n";
    }
}

// -------------------- COMPARE --------------------
void compareAlgorithms(const vector<Subject>& subjects, int totalTime, int totalDays){
    if(subjects.empty()){cout << "No subjects to compare!\n"; return;}
    cout << "\nALGORITHM COMPARISON\n";
    cout << string(80,'=') << "\n";
    cout << setw(20) << "Algorithm" << setw(20) << "Time Complexity" << setw(20) << "Space Complexity" << setw(20) << "Best For\n";
    cout << string(80,'-') << "\n";
    cout << setw(20) << "0/1 Knapsack (DP)" << setw(20) << "O(n*W)" << setw(20) << "O(n*W)" << setw(20) << "Optimal Importance\n";
    cout << setw(20) << "Greedy" << setw(20) << "O(n log n)" << setw(20) << "O(n)" << setw(20) << "Fast Approx\n";
    cout << setw(20) << "Job Sequencing" << setw(20) << "O(n^2)" << setw(20) << "O(n)" << setw(20) << "Meet Deadlines\n";
    cout << string(80,'=') << "\n";
}

// -------------------- LOGIN FILE --------------------
vector<Officer> readLoginFile(){
    vector<Officer> officers;
    ifstream inFile(USER_FILE);
    if(!inFile) return officers;
    string line;
    while(getline(inFile,line)){
        stringstream ss(line);
        string u,p;
        if(getline(ss,u,',') && getline(ss,p))
            officers.emplace_back(u,p);
    }
    return officers;
}

void saveOfficer(const Officer& officer){
    ofstream outFile(USER_FILE, ios::app);
    if(!outFile) return;
    outFile << officer.username << "," << officer.password << "\n";
}

bool createAccount(){
    string username,password;
    cin.ignore();
    cout << "Enter new username: "; getline(cin,username);
    cout << "Enter password: "; getline(cin,password);

    vector<Officer> officers = readLoginFile();
    for(auto& o:officers)
        if(o.username == username){
            cout << "Username exists!\n"; return false;
        }
    saveOfficer(Officer(username,password));
    cout << "Account created successfully!\n"; return true;
}

bool login(string &username){
    string password;
    cin.ignore();
    cout << "Username: "; getline(cin,username);
    cout << "Password: "; getline(cin,password);

    vector<Officer> officers = readLoginFile();
    for(auto &o:officers)
        if(o.username==username && o.password==password) return true;
    cout << "Invalid username or password!\n"; return false;
}

// -------------------- SUBJECT FILE --------------------
void saveSubjectToFile(const Subject& s){
    ofstream outFile(SUBJECT_FILE, ios::app);
    if(!outFile) return;
    outFile << s.username << "," << s.name << "," << s.importance << ","
            << s.timeRequired << "," << s.deadline << "\n";
}

vector<Subject> readSubjectsForUser(const string& username){
    vector<Subject> subjects;
    ifstream inFile(SUBJECT_FILE);
    if(!inFile) return subjects;

    string line;
    while(getline(inFile,line)){
        stringstream ss(line);
        Subject s; string temp;
        getline(ss,s.username,',');
        if(s.username!=username) continue;
        getline(ss,s.name,',');
        getline(ss,temp,','); s.importance=stoi(temp);
        getline(ss,temp,','); s.timeRequired=stoi(temp);
        getline(ss,temp,','); s.deadline=stoi(temp);
        subjects.push_back(s);
    }
    return subjects;
}

void overwriteAllSubjects(const vector<Subject>& subjects){
    ofstream outFile(SUBJECT_FILE);
    if(!outFile) return;
    for(const auto& s:subjects){
        outFile << s.username << "," << s.name << "," << s.importance << ","
                << s.timeRequired << "," << s.deadline << "\n";
    }
}
