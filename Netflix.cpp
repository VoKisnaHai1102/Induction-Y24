#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
using namespace std

// Forward declarations
class Content;
class Movie;
class TVShow;
class User;
class Admin;
class System;

// Utility functions... could not do this by myself. Had to take help of Claude for this part
string getCurrentDate() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    
    ostringstream oss;
    oss << setfill('0') << setw(2) << localTime->tm_mday << "/"
        << setfill('0') << setw(2) << (localTime->tm_mon + 1) << "/"
        << (localTime->tm_year + 1900);
    
    return oss.str();
}

string getFutureDate(int daysFromNow) {
    time_t now = time(nullptr);
    now += daysFromNow * 24 * 60 * 60; // Add days in seconds
    tm* futureTime = localtime(&now);
    
    ostringstream oss;
    oss << setfill('0') << setw(2) << futureTime->tm_mday << "/"
        << setfill('0') << setw(2) << (futureTime->tm_mon + 1) << "/"
        << (futureTime->tm_year + 1900);
    
    return oss.str();
}

//  Content class
class Content {
protected:
    string title;
    string genre;
    double rating;
    bool is_rented;
    bool is_purchased;
    string owner; // Username of the person who rented/purchased
    string rent_date;
    string return_date;

public:
    Content(const string& title, const string& genre, double rating)
        : title(title), genre(genre), rating(rating), is_rented(false), 
          is_purchased(false), owner(""), rent_date(""), return_date("") {}
    
    virtual ~Content() {}
    
    // Getters
    string getTitle() const { return title; }
    string getGenre() const { return genre; }
    double getRating() const { return rating; }
    bool isRented() const { return is_rented; }
    bool isPurchased() const { return is_purchased; }
    string getOwner() const { return owner; }
    string getRentDate() const { return rent_date; }
    string getReturnDate() const { return return_date; }
    
    // Rent/Purchase methods
    virtual bool rent(const string& username) {
        if (is_rented || is_purchased) {
            return false;
        }
        is_rented = true;
        owner = username;
        rent_date = getCurrentDate();
        return_date = getFutureDate(7); // Default 7-day rental
        return true;
    }
    
    virtual bool purchase(const string& username) {
        if (is_purchased) {
            return false;
        }
        if (is_rented && owner != username) {
            return false; // Cannot purchase if someone else has rented
        }
        is_purchased = true;
        is_rented = false; // No longer rented if purchased
        owner = username;
        rent_date = "";
        return_date = "";
        return true;
    }
    
    virtual bool returnContent() {
        if (!is_rented) {
            return false;
        }
        is_rented = false;
        owner = "";
        rent_date = "";
        return_date = "";
        return true;
    }
    
    virtual string getType() const = 0;
    virtual double getRentCost() const = 0;
    virtual double getPurchaseCost() const = 0;
    virtual void display() const = 0;
};

// Movie class
class Movie : public Content {
private:
    int duration; // in minutes
    double rent_cost;
    double purchase_cost;

public:
    Movie(const string& title, const string& genre, double rating, 
         int duration, double rent_cost, double purchase_cost)
        : Content(title, genre, rating), duration(duration), 
          rent_cost(rent_cost), purchase_cost(purchase_cost) {}
    
    string getType() const override { return "Movie"; }
    double getRentCost() const override { return rent_cost; }
    double getPurchaseCost() const override { return purchase_cost; }
    int getDuration() const { return duration; }
    
    void display() const override {
        cout << "Movie: " << title << endl;
        cout << "Genre: " << genre << endl;
        cout << "Rating: " << rating << "/10" << endl;
        cout << "Duration: " << duration << " minutes" << endl;
        cout << "Rent Cost: $" << rent_cost << endl;
        cout << "Purchase Cost: $" << purchase_cost << endl;
        cout << "Status: " << (is_purchased ? "Purchased" : (is_rented ? "Rented" : "Available")) << endl;
        if (is_rented) {
            cout << "Rented by: " << owner << endl;
            cout << "Rent Date: " << rent_date << endl;
            cout << "Return Date: " << return_date << endl;
        }
        cout << endl;
    }
};

// TV Show class
class TVShow : public Content {
private:
    int seasons;
    int episodes_per_season;
    double season_rent_cost;
    double season_purchase_cost;

public:
    TVShow(const string& title, const string& genre, double rating,
          int seasons, int episodes_per_season, 
          double season_rent_cost, double season_purchase_cost)
        : Content(title, genre, rating), seasons(seasons),
          episodes_per_season(episodes_per_season),
          season_rent_cost(season_rent_cost),
          season_purchase_cost(season_purchase_cost) {}
    
    string getType() const override { return "TV Show"; }
    double getRentCost() const override { return season_rent_cost * seasons; }
    double getPurchaseCost() const override { return season_purchase_cost * seasons; }
    int getSeasons() const { return seasons; }
    int getEpisodesPerSeason() const { return episodes_per_season; }
    double getSeasonRentCost() const { return season_rent_cost; }
    double getSeasonPurchaseCost() const { return season_purchase_cost; }
    
    void display() const override {
        cout << "TV Show: " << title << endl;
        cout << "Genre: " << genre << endl;
        cout << "Rating: " << rating << "/10" << endl;
        cout << "Seasons: " << seasons << endl;
        cout << "Episodes per Season: " << episodes_per_season << endl;
        cout << "Rent Cost per Season: $" << season_rent_cost << endl;
        cout << "Total Rent Cost: $" << getRentCost() << endl;
        cout << "Purchase Cost per Season: $" << season_purchase_cost << endl;
        cout << "Total Purchase Cost: $" << getPurchaseCost() << endl;
        cout << "Status: " << (is_purchased ? "Purchased" : (is_rented ? "Rented" : "Available")) << endl;
        if (is_rented) {
            cout << "Rented by: " << owner << endl;
            cout << "Rent Date: " << rent_date << endl;
            cout << "Return Date: " << return_date << endl;
        }
        cout << endl;
    }
};

// User class
class User {
protected:
    string username;
    string password;
    vector<shared_ptr<Content>> rented_content;
    vector<shared_ptr<Content>> purchased_content;
    double charges_due;

public:
    User(const string& username, const string& password)
        : username(username), password(password), charges_due(0.0) {}
    
    virtual ~User() {}
    
    string getUsername() const { return username; }
    double getChargesDue() const { return charges_due; }
    
    bool validatePassword(const string& input_password) const {
        return password == input_password;
    }
    
    bool hasRentedContent() const {
        return !rented_content.empty();
    }
    
    bool rentContent(shared_ptr<Content> content) {
        if (content->rent(username)) {
            rented_content.push_back(content);
            charges_due += content->getRentCost();
            return true;
        }
        return false;
    }
    
    bool purchaseContent(shared_ptr<Content> content) {
        if (content->purchase(username)) {
            // If previously rented, remove from rented list
            auto it = find(rented_content.begin(), rented_content.end(), content);
            if (it != rented_content.end()) {
                rented_content.erase(it);
            }
            
            purchased_content.push_back(content);
            charges_due += content->getPurchaseCost();
            return true;
        }
        return false;
    }
    
    bool returnContent(shared_ptr<Content> content) {
        auto it = find(rented_content.begin(), rented_content.end(), content);
        if (it != rented_content.end() && content->returnContent()) {
            rented_content.erase(it);
            return true;
        }
        return false;
    }
    
    void displayRentedContent() const {
        if (rented_content.empty()) {
            cout << "You haven't rented any content yet." << endl;
            return;
        }
        
        cout << "---- Your Rented Content ----" << endl;
        for (const auto& content : rented_content) {
            cout << "Title: " << content->getTitle() << endl;
            cout << "Type: " << content->getType() << endl;
            cout << "Rented on: " << content->getRentDate() << endl;
            cout << "Return by: " << content->getReturnDate() << endl;
            cout << "Cost: $" << content->getRentCost() << endl;
            cout << endl;
        }
    }
    
    void displayPurchasedContent() const {
        if (purchased_content.empty()) {
            cout << "You haven't purchased any content yet." << endl;
            return;
        }
        
        cout << "---- Your Purchased Content ----" << endl;
        for (const auto& content : purchased_content) {
            cout << "Title: " << content->getTitle() << endl;
            cout << "Type: " << content->getType() << endl;
            cout << "Cost: $" << content->getPurchaseCost() << endl;
            cout << endl;
        }
    }
    
    void displayChargesDue() const {
        cout << "Total charges due: $" << charges_due << endl;
    }
    
    virtual void displayMenu() const {
        cout << "\n----- User Menu -----" << endl;
        cout << "1. Browse content by category" << endl;
        cout << "2. Browse content by genre" << endl;
        cout << "3. Search content by title" << endl;
        cout << "4. Search content by genre" << endl;
        cout << "5. Rent content" << endl;
        cout << "6. Purchase content" << endl;
        cout << "7. Return rented content" << endl;
        cout << "8. View rented content" << endl;
        cout << "9. View purchased content" << endl;
        cout << "10. Check charges due" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter your choice: ";
    }
};

// Admin class
class Admin : public User {
public:
    Admin(const string& username, const string& password)
        : User(username, password) {}
    
    void displayMenu() const override {
        cout << "\n----- Admin Menu -----" << endl;
        cout << "1. Add new movie" << endl;
        cout << "2. Add new TV show" << endl;
        cout << "3. Remove content" << endl;
        cout << "4. Check charges due for user" << endl;
        cout << "5. Browse all content" << endl;
        cout << "6. Search content" << endl;
        cout << "0. Logout" << endl;
        cout << "Enter your choice: ";
    }
};

// Netflix-like system
class System {
private:
    vector<shared_ptr<Content>> content_database;
    map<string, shared_ptr<User>> users;
    shared_ptr<User> current_user;
    
    // Initialize with sample data
    void initializeDatabase() {
        // Add 10 sample movies
        addContent(make_shared<Movie>("The Shawshank Redemption", "Drama", 9.3, 142, 3.99, 14.99));
        addContent(make_shared<Movie>("The Godfather", "Crime", 9.2, 175, 3.99, 14.99));
        addContent(make_shared<Movie>("The Dark Knight", "Action", 9.0, 152, 4.99, 15.99));
        addContent(make_shared<Movie>("Pulp Fiction", "Crime", 8.9, 154, 3.99, 12.99));
        addContent(make_shared<Movie>("Fight Club", "Drama", 8.8, 139, 3.99, 12.99));
        addContent(make_shared<Movie>("Inception", "Sci-Fi", 8.8, 148, 4.99, 15.99));
        addContent(make_shared<Movie>("The Matrix", "Sci-Fi", 8.7, 136, 3.99, 13.99));
        addContent(make_shared<Movie>("Interstellar", "Sci-Fi", 8.6, 169, 4.99, 16.99));
        addContent(make_shared<Movie>("Parasite", "Thriller", 8.5, 132, 4.99, 16.99));
        addContent(make_shared<Movie>("Joker", "Drama", 8.4, 122, 4.99, 15.99));
        
        // Add some sample TV shows
        addContent(make_shared<TVShow>("Breaking Bad", "Drama", 9.5, 5, 13, 2.99, 9.99));
        addContent(make_shared<TVShow>("Game of Thrones", "Fantasy", 9.2, 8, 10, 2.99, 12.99));
        addContent(make_shared<TVShow>("Stranger Things", "Sci-Fi", 8.7, 4, 8, 2.99, 9.99));
        addContent(make_shared<TVShow>("The Office", "Comedy", 8.9, 9, 24, 1.99, 7.99));
        addContent(make_shared<TVShow>("Friends", "Comedy", 8.5, 10, 24, 1.99, 8.99));
        
        // Add admin account
        users["admin"] = make_shared<Admin>("admin", "admin123");
    }
    
public:
    System() : current_user(nullptr) {
        initializeDatabase();
    }
    
    void addContent(shared_ptr<Content> content) {
        content_database.push_back(content);
    }
    
    bool removeContent(const string& title) {
        auto it = find_if(content_database.begin(), content_database.end(),
                              [&title](const shared_ptr<Content>& c) {
                                  return c->getTitle() == title;
                              });
        
        if (it != content_database.end()) {
            content_database.erase(it);
            return true;
        }
        return false;
    }
    
    shared_ptr<Content> findContentByTitle(const string& title) {
        auto it = find_if(content_database.begin(), content_database.end(),
                              [&title](const shared_ptr<Content>& c) {
                                  return c->getTitle() == title;
                              });
        
        if (it != content_database.end()) {
            return *it;
        }
        return nullptr;
    }
    
    vector<shared_ptr<Content>> findContentByGenre(const string& genre) {
        vector<shared_ptr<Content>> results;
        
        for (const auto& content : content_database) {
            if (content->getGenre() == genre) {
                results.push_back(content);
            }
        }
        
        return results;
    }
    
    vector<shared_ptr<Content>> getContentByType(const string& type) {
        vector<shared_ptr<Content>> results;
        
        for (const auto& content : content_database) {
            if (content->getType() == type) {
                results.push_back(content);
            }
        }
        
        return results;
    }
    
    vector<string> getUniqueGenres() {
        vector<string> genres;
        
        for (const auto& content : content_database) {
            if (find(genres.begin(), genres.end(), content->getGenre()) == genres.end()) {
                genres.push_back(content->getGenre());
            }
        }
        
        return genres;
    }
    
    bool isUsernameAvailable(const string& username) {
        return users.find(username) == users.end();
    }
    
    bool registerUser(const string& username, const string& password) {
        if (!isUsernameAvailable(username)) {
            return false;
        }
        
        users[username] = make_shared<User>(username, password);
        return true;
    }
    
    bool login(const string& username, const string& password) {
        auto it = users.find(username);
        if (it != users.end() && it->second->validatePassword(password)) {
            current_user = it->second;
            return true;
        }
        return false;
    }
    
    void logout() {
        current_user = nullptr;
    }
    
    bool isAdminLoggedIn() const {
        return current_user && dynamic_cast<Admin*>(current_user.get()) != nullptr;
    }
    
    bool isUserLoggedIn() const {
        return current_user != nullptr;
    }
    
    shared_ptr<User> getCurrentUser() const {
        return current_user;
    }
    
    shared_ptr<User> getUserByUsername(const string& username) {
        auto it = users.find(username);
        if (it != users.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void displayAllContent() const {
        cout << "----- Content Database -----" << endl;
        int index = 1;
        for (const auto& content : content_database) {
            cout << index++ << ". " << content->getTitle() 
                     << " (" << content->getType() << ", " << content->getGenre() << ")" 
                     << " - " << (content->isRented() || content->isPurchased() ? "Not Available" : "Available")
                     << endl;
        }
        cout << endl;
    }
    
    void run() {
        int choice;
        
        while (true) {
            if (!isUserLoggedIn()) {
                displayMainMenu();
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                switch (choice) {
                    case 1:
                        handleLogin();
                        break;
                    case 2:
                        handleSignUp();
                        break;
                    case 3:
                        handleAdminLogin();
                        break;
                    case 0:
                        cout << "Thank you for using Netflix-Style Inventory System. Goodbye!" << endl;
                        return;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            } else if (isAdminLoggedIn()) {
                current_user->displayMenu();
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                switch (choice) {
                    case 1:
                        handleAddMovie();
                        break;
                    case 2:
                        handleAddTVShow();
                        break;
                    case 3:
                        handleRemoveContent();
                        break;
                    case 4:
                        handleCheckUserCharges();
                        break;
                    case 5:
                        displayAllContent();
                        break;
                    case 6:
                        handleAdminSearch();
                        break;
                    case 0:
                        logout();
                        cout << "Logged out successfully." << endl;
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                current_user->displayMenu();
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                switch (choice) {
                    case 1:
                        handleBrowseByCategory();
                        break;
                    case 2:
                        handleBrowseByGenre();
                        break;
                    case 3:
                        handleSearchByTitle();
                        break;
                    case 4:
                        handleSearchByGenre();
                        break;
                    case 5:
                        handleRentContent();
                        break;
                    case 6:
                        handlePurchaseContent();
                        break;
                    case 7:
                        handleReturnContent();
                        break;
                    case 8:
                        current_user->displayRentedContent();
                        break;
                    case 9:
                        current_user->displayPurchasedContent();
                        break;
                    case 10:
                        current_user->displayChargesDue();
                        break;
                    case 0:
                        logout();
                        cout
                        << "Logged out successfully." << endl;
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
    
private:
    void displayMainMenu() const {
        cout << "\n----- Netflix-Style Inventory System -----" << endl;
        cout << "1. User Login" << endl;
        cout << "2. User Sign Up" << endl;
        cout << "3. Admin Login" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
    }
    
    void handleLogin() {
        string username, password;
        
        cout << "Enter username: ";
        getline(cin, username);
        cout << "Enter password: ";
        getline(cin, password);
        
        if (login(username, password)) {
            cout << "Login successful. Welcome, " << username << "!" << endl;
        } else {
            cout << "Login failed. Invalid username or password." << endl;
        }
    }
    
    void handleSignUp() {
        string username, password;
        
        cout << "Enter new username: ";
        getline(cin, username);
        
        if (!isUsernameAvailable(username)) {
            cout << "Username already taken. Please choose another one." << endl;
            return;
        }
        
        cout << "Enter password: ";
        getline(cin, password);
        
        if (registerUser(username, password)) {
            cout << "Registration successful! You can now login." << endl;
        } else {
            cout << "Registration failed. Please try again." << endl;
        }
    }
    
    void handleAdminLogin() {
        string username, password;
        
        cout << "Enter admin username: ";
        getline(cin, username);
        cout << "Enter admin password: ";
        getline(cin, password);
        
        if (login(username, password) && isAdminLoggedIn()) {
            cout << "Admin login successful. Welcome, " << username << "!" << endl;
        } else {
            cout << "Admin login failed. Invalid username or password." << endl;
            logout(); // In case a regular user tried to login as admin
        }
    }
    
    void handleBrowseByCategory() {
        cout << "\n----- Browse by Category -----" << endl;
        cout << "1. Movies" << endl;
        cout << "2. TV Shows" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        vector<shared_ptr<Content>> results;
        
        switch (choice) {
            case 1:
                results = getContentByType("Movie");
                break;
            case 2:
                results = getContentByType("TV Show");
                break;
            default:
                cout << "Invalid choice." << endl;
                return;
        }
        
        displayContentList(results);
    }
    
    void handleBrowseByGenre() {
        vector<string> genres = getUniqueGenres();
        
        cout << "\n----- Browse by Genre -----" << endl;
        for (size_t i = 0; i < genres.size(); ++i) {
            cout << (i + 1) << ". " << genres[i] << endl;
        }
        
        cout << "Enter your choice (1-" << genres.size() << "): ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice < 1 || choice > static_cast<int>(genres.size())) {
            cout << "Invalid choice." << endl;
            return;
        }
        
        vector<shared_ptr<Content>> results = findContentByGenre(genres[choice - 1]);
        displayContentList(results);
    }
    
    void handleSearchByTitle() {
        string title;
        
        cout << "Enter title to search for: ";
        getline(cin, title);
        
        shared_ptr<Content> content = findContentByTitle(title);
        
        if (content) {
            content->display();
        } else {
            cout << "Content not found." << endl;
        }
    }
    
    void handleSearchByGenre() {
        string genre;
        
        cout << "Enter genre to search for: ";
        getline(cin, genre);
        
        vector<shared_ptr<Content>> results = findContentByGenre(genre);
        
        if (results.empty()) {
            cout << "No content found for genre: " << genre << endl;
        } else {
            displayContentList(results);
        }
    }
    
    void displayContentList(const vector<shared_ptr<Content>>& content_list) {
        if (content_list.empty()) {
            cout << "No content available." << endl;
            return;
        }
        
        cout << "\n----- Content List -----" << endl;
        int index = 1;
        for (const auto& content : content_list) {
            cout << index++ << ". " << content->getTitle() 
                     << " (" << content->getType() << ", " << content->getGenre() << ")" 
                     << " - " << (content->isRented() || content->isPurchased() ? "Not Available" : "Available")
                     << endl;
        }
        
        cout << "\nEnter the number to view details (0 to cancel): ";
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice > 0 && choice <= static_cast<int>(content_list.size())) {
            content_list[choice - 1]->display();
        }
    }
    
    void handleRentContent() {
        string title;
        
        cout << "Enter the title of the content you want to rent: ";
        getline(cin, title);
        
        shared_ptr<Content> content = findContentByTitle(title);
        
        if (!content) {
            cout << "Content not found." << endl;
            return;
        }
        
        if (content->isRented() || content->isPurchased()) {
            cout << "This content is not available for rent." << endl;
            return;
        }
        
        cout << "Rent cost: $" << content->getRentCost() << endl;
        cout << "Do you want to rent this content? (y/n): ";
        
        char response;
        cin >> response;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (response == 'y' || response == 'Y') {
            if (current_user->rentContent(content)) {
                cout << "Content rented successfully!" << endl;
            } else {
                cout << "Failed to rent content." << endl;
            }
        }
    }
    
    void handlePurchaseContent() {
        string title;
        
        cout << "Enter the title of the content you want to purchase: ";
        getline(cin, title);
        
        shared_ptr<Content> content = findContentByTitle(title);
        
        if (!content) {
            cout << "Content not found." << endl;
            return;
        }
        
        if (content->isPurchased()) {
            cout << "This content is not available for purchase." << endl;
            return;
        }
        
        cout << "Purchase cost: $" << content->getPurchaseCost() << endl;
        cout << "Do you want to purchase this content? (y/n): ";
        
        char response;
        cin >> response;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (response == 'y' || response == 'Y') {
            if (current_user->purchaseContent(content)) {
                cout << "Content purchased successfully!" << endl;
            } else {
                cout << "Failed to purchase content." << endl;
            }
        }
    }
    
    void handleReturnContent() {
        current_user->displayRentedContent();
        
        if (current_user->hasRentedContent() == false) {
            return;
        }
        
        string title;
        cout << "Enter the title of the content you want to return: ";
        getline(cin, title);
        
        shared_ptr<Content> content = findContentByTitle(title);
        
        if (!content) {
            cout << "Content not found." << endl;
            return;
        }
        
        if (current_user->returnContent(content)) {
            cout << "Content returned successfully!" << endl;
        } else {
            cout << "Failed to return content. Make sure you've rented it." << endl;
        }
    }
    
    void handleAddMovie() {
        string title, genre;
        double rating, rent_cost, purchase_cost;
        int duration;
        
        cout << "Enter movie title: ";
        getline(cin, title);
        
        if (findContentByTitle(title)) {
            cout << "A movie with this title already exists." << endl;
            return;
        }
        
        cout << "Enter genre: ";
        getline(cin, genre);
        
        cout << "Enter rating (0-10): ";
        cin >> rating;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (rating < 0 || rating > 10) {
            cout << "Invalid rating. Must be between 0 and 10." << endl;
            return;
        }
        
        cout << "Enter duration (in minutes): ";
        cin >> duration;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (duration <= 0) {
            cout << "Invalid duration." << endl;
            return;
        }
        
        cout << "Enter rent cost: $";
        cin >> rent_cost;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (rent_cost < 0) {
            cout << "Invalid rent cost." << endl;
            return;
        }
        
        cout << "Enter purchase cost: $";
        cin >> purchase_cost;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (purchase_cost < 0) {
            cout << "Invalid purchase cost." << endl;
            return;
        }
        
        addContent(make_shared<Movie>(title, genre, rating, duration, rent_cost, purchase_cost));
        cout << "Movie added successfully!" << endl;
    }
    
    void handleAddTVShow() {
        string title, genre;
        double rating, season_rent_cost, season_purchase_cost;
        int seasons, episodes_per_season;
        
        cout << "Enter TV show title: ";
        getline(cin, title);
        
        if (findContentByTitle(title)) {
            cout << "A TV show with this title already exists." << endl;
            return;
        }
        
        cout << "Enter genre: ";
        getline(cin, genre);
        
        cout << "Enter rating (0-10): ";
        cin >> rating;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (rating < 0 || rating > 10) {
            cout << "Invalid rating. Must be between 0 and 10." << endl;
            return;
        }
        
        cout << "Enter number of seasons: ";
        cin >> seasons;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (seasons <= 0) {
            cout << "Invalid number of seasons." << endl;
            return;
        }
        
        cout << "Enter episodes per season: ";
        cin >> episodes_per_season;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (episodes_per_season <= 0) {
            cout << "Invalid number of episodes." << endl;
            return;
        }
        
        cout << "Enter rent cost per season: $";
        cin >> season_rent_cost;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (season_rent_cost < 0) {
            cout << "Invalid rent cost." << endl;
            return;
        }
        
        cout << "Enter purchase cost per season: $";
        cin >> season_purchase_cost;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (season_purchase_cost < 0) {
            cout << "Invalid purchase cost." << endl;
            return;
        }
        
        addContent(make_shared<TVShow>(title, genre, rating, seasons, episodes_per_season, 
                                           season_rent_cost, season_purchase_cost));
        cout << "TV show added successfully!" << endl;
    }
    
    void handleRemoveContent() {
        displayAllContent();
        
        string title;
        cout << "Enter the title of the content you want to remove: ";
        getline(cin, title);
        
        if (removeContent(title)) {
            cout << "Content removed successfully!" << endl;
        } else {
            cout << "Failed to remove content. Title not found." << endl;
        }
    }
    
    void handleCheckUserCharges() {
        string username;
        cout << "Enter username: ";
        getline(cin, username);
        
        shared_ptr<User> user = getUserByUsername(username);
        
        if (user) {
            cout << "Charges due for user " << username << ": $" << user->getChargesDue() << endl;
        } else {
            cout << "User not found." << endl;
        }
    }
    
    void handleAdminSearch() {
        cout << "\n----- Search Content -----" << endl;
        cout << "1. Search by title" << endl;
        cout << "2. Search by genre" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                handleSearchByTitle();
                break;
            case 2:
                handleSearchByGenre();
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }
};
// did not do hashing as did not have enough time for encryption. Created classes  myself and adding things like genre, rating etc throught GPT. 
int main() {
    System netflix_system;
    netflix_system.run();
    return 0;
}
