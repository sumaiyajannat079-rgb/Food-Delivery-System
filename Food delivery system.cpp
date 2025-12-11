
#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;

// ====================== Data Structures ======================

struct Order {
    string orderId;
    string customerAddress;
    vector<string> items;
    string status; // "pending", "active", "completed"
    string assignedDriverId;
    time_t orderTime;

    Order(string id, string address, vector<string> itemList)
        : orderId(id), customerAddress(address), items(itemList),
          status("pending"), assignedDriverId(""), orderTime(time(0)) {}
};

struct Driver {
    string driverId;
    string name;
    time_t nextAvailableTime; // When driver becomes available

    Driver(string id, string driverName)
        : driverId(id), name(driverName), nextAvailableTime(time(0)) {}

    // For min-heap comparison
    bool operator>(const Driver& other) const {
        return nextAvailableTime > other.nextAvailableTime;
    }
};

// ====================== Delivery System Class ======================

class DeliverySystem {
private:
    // Data structures as specified
    queue<Order*> orderQueue;  // FIFO for incoming orders
    priority_queue<Driver, vector<Driver>, greater<Driver>> driverHeap;  // Min-heap for drivers
    unordered_map<string, Order*> orderMap;  // Hash map for order tracking

    vector<Driver> allDrivers;  // Store all drivers
    vector<Order*> completedOrders;  // Store completed orders

    int orderCounter;

public:
    DeliverySystem() : orderCounter(1) {
        initializeDrivers();
    }

    ~DeliverySystem() {
        // Cleanup
        for (auto& pair : orderMap) {
            delete pair.second;
        }
        for (auto order : completedOrders) {
            delete order;
        }
    }

    // ====================== Module 1: Place Order ======================
    void placeOrder(const string& address, const vector<string>& items) {
        string orderId = "ORD" + to_string(orderCounter++);
        Order* newOrder = new Order(orderId, address, items);

        // Enqueue the order
        orderQueue.push(newOrder);

        // Add to tracking hash map
        orderMap[orderId] = newOrder;

        cout << "\n✅ Order placed successfully!" << endl;
        cout << "   Order ID: " << orderId << endl;
        cout << "   Address: " << address << endl;
        cout << "   Items: ";
        for (const auto& item : items) {
            cout << item << " ";
        }
        cout << endl;
        cout << "   Status: " << newOrder->status << endl;
        cout << "   Time: " << formatTime(newOrder->orderTime) << endl;
    }

    // ====================== Module 2: Assign Delivery Driver ======================
    void assignDriverToOrder() {
        if (orderQueue.empty()) {
            cout << "\n❌ No pending orders to assign." << endl;
            return;
        }

        if (driverHeap.empty()) {
            cout << "\n❌ No available drivers." << endl;
            return;
        }

        // Get next order (FIFO)
        Order* order = orderQueue.front();
        orderQueue.pop();

        // Extract driver with lowest availability time (min-heap)
        Driver driver = driverHeap.top();
        driverHeap.pop();

        // Calculate delivery time (simulate delivery time)
        time_t currentTime = time(0);
        time_t deliveryTime = currentTime + 1800; // 30 minutes for delivery

        // Update order
        order->status = "active";
        order->assignedDriverId = driver.driverId;

        // Update driver
        driver.nextAvailableTime = deliveryTime;

        // Put driver back in heap with updated time
        driverHeap.push(driver);

        // Update driver in allDrivers list
        for (auto& d : allDrivers) {
            if (d.driverId == driver.driverId) {
                d.nextAvailableTime = deliveryTime;
                break;
            }
        }

        cout << "\n✅ Driver assigned successfully!" << endl;
        cout << "   Order ID: " << order->orderId << endl;
        cout << "   Assigned Driver: " << driver.name << " (ID: " << driver.driverId << ")" << endl;
        cout << "   Estimated Delivery: " << formatTime(deliveryTime) << endl;
        cout << "   Order Status: " << order->status << endl;
    }

    // ====================== Module 3: Track Active Orders ======================
    void trackOrder(const string& orderId) {
        auto it = orderMap.find(orderId);
        if (it == orderMap.end()) {
            cout << "\n❌ Order not found: " << orderId << endl;
            return;
        }

        Order* order = it->second;

        cout << "\n📋 Order Tracking" << endl;
        cout << "   Order ID: " << order->orderId << endl;
        cout << "   Address: " << order->customerAddress << endl;
        cout << "   Items: ";
        for (const auto& item : order->items) {
            cout << item << " ";
        }
        cout << endl;
        cout << "   Status: " << order->status << endl;
        cout << "   Order Time: " << formatTime(order->orderTime) << endl;

        if (order->status == "active" && !order->assignedDriverId.empty()) {
            cout << "   Assigned Driver ID: " << order->assignedDriverId << endl;

            // Find driver name
            for (const auto& driver : allDrivers) {
                if (driver.driverId == order->assignedDriverId) {
                    cout << "   Driver Name: " << driver.name << endl;
                    cout << "   Next Available: " << formatTime(driver.nextAvailableTime) << endl;
                    break;
                }
            }
        }
    }

    // ====================== Module 4: Complete Delivery ======================
    void completeDelivery(const string& orderId) {
        auto it = orderMap.find(orderId);
        if (it == orderMap.end()) {
            cout << "\n❌ Order not found: " << orderId << endl;
            return;
        }

        Order* order = it->second;

        if (order->status == "completed") {
            cout << "\nℹ️ Order already completed: " << orderId << endl;
            return;
        }

        if (order->status == "pending") {
            cout << "\n❌ Order not yet assigned to a driver: " << orderId << endl;
            return;
        }

        // Mark order as completed
        order->status = "completed";

        // Find and update the driver's availability
        if (!order->assignedDriverId.empty()) {
            for (auto& driver : allDrivers) {
                if (driver.driverId == order->assignedDriverId) {
                    // Driver becomes available immediately
                    driver.nextAvailableTime = time(0);

                    // Update driver in heap
                    updateDriverInHeap(driver);

                    cout << "   Driver " << driver.name << " is now available." << endl;
                    break;
                }
            }
        }

        // Move to completed orders
        completedOrders.push_back(order);

        cout << "\n✅ Delivery completed successfully!" << endl;
        cout << "   Order ID: " << order->orderId << endl;
        cout << "   Completion Time: " << formatTime(time(0)) << endl;
    }

    // ====================== Module 5: View Order Summary ======================
    void viewOrderSummary() {
        cout << "\n📊 ORDER SUMMARY" << endl;
        cout << "==========================================" << endl;

        // Pending Orders (in queue)
        cout << "\n⏳ PENDING ORDERS (in queue): " << orderQueue.size() << endl;
        cout << "------------------------------------------" << endl;

        // Note: Can't iterate queue directly, so we need a copy
        queue<Order*> tempQueue = orderQueue;
        int pendingCount = 1;
        while (!tempQueue.empty()) {
            Order* order = tempQueue.front();
            tempQueue.pop();
            cout << pendingCount++ << ". ID: " << order->orderId
                 << " | Address: " << order->customerAddress
                 << " | Items: " << order->items.size() << endl;
        }

        // Active Orders (in hash map but not completed)
        cout << "\n🚚 ACTIVE ORDERS (assigned to drivers):" << endl;
        cout << "------------------------------------------" << endl;
        int activeCount = 0;
        for (const auto& pair : orderMap) {
            Order* order = pair.second;
            if (order->status == "active") {
                activeCount++;
                cout << activeCount << ". ID: " << order->orderId
                     << " | Driver: " << order->assignedDriverId
                     << " | Address: " << order->customerAddress << endl;
            }
        }
        if (activeCount == 0) {
            cout << "No active orders." << endl;
        }

        // Completed Orders
        cout << "\n✅ COMPLETED ORDERS: " << completedOrders.size() << endl;
        cout << "------------------------------------------" << endl;
        for (size_t i = 0; i < min((size_t)5, completedOrders.size()); i++) {
            Order* order = completedOrders[i];
            cout << i+1 << ". ID: " << order->orderId
                 << " | Address: " << order->customerAddress
                 << " | Items: " << order->items.size() << endl;
        }
        if (completedOrders.size() > 5) {
            cout << "... and " << (completedOrders.size() - 5) << " more." << endl;
        }

        // Driver Status
        cout << "\n👨‍🍳 DRIVER STATUS:" << endl;
        cout << "------------------------------------------" << endl;
        for (const auto& driver : allDrivers) {
            string status = (driver.nextAvailableTime <= time(0)) ? "Available" : "Busy until " + formatTime(driver.nextAvailableTime);
            cout << driver.name << " (ID: " << driver.driverId << "): " << status << endl;
        }

        cout << "\n==========================================" << endl;
    }

    // ====================== Helper Functions ======================
private:
    void initializeDrivers() {
        vector<string> driverNames = {"John", "Sarah", "Mike", "Emma", "David"};

        for (int i = 0; i < driverNames.size(); i++) {
            string driverId = "DRV" + to_string(i + 1);
            Driver driver(driverId, driverNames[i]);
            allDrivers.push_back(driver);
            driverHeap.push(driver);
        }

        cout << "✅ System initialized with " << driverNames.size() << " drivers." << endl;
    }

    string formatTime(time_t timestamp) {
        tm* timeinfo = localtime(&timestamp);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
        return string(buffer);
    }

    void updateDriverInHeap(const Driver& updatedDriver) {
        // Rebuild heap with updated driver info
        priority_queue<Driver, vector<Driver>, greater<Driver>> newHeap;

        for (auto driver : allDrivers) {
            newHeap.push(driver);
        }

        driverHeap = newHeap;
    }

    // ====================== Public Helper Methods ======================
public:
    void showPendingQueue() {
        cout << "\n📋 Current Pending Queue (FIFO):" << endl;
        cout << "------------------------------------------" << endl;

        if (orderQueue.empty()) {
            cout << "Queue is empty." << endl;
            return;
        }

        queue<Order*> tempQueue = orderQueue;
        int count = 1;
        while (!tempQueue.empty()) {
            Order* order = tempQueue.front();
            tempQueue.pop();
            cout << count++ << ". Order ID: " << order->orderId
                 << " | Address: " << order->customerAddress
                 << " | Items: " << order->items.size() << endl;
        }
    }

    int getPendingOrderCount() const {
        return orderQueue.size();
    }
};

// ====================== Main Function with Menu ======================

void displayMenu() {
    cout << "\n==========================================" << endl;
    cout << "   ONLINE FOOD DELIVERY SYSTEM" << endl;
    cout << "==========================================" << endl;
    cout << "1. Place New Order" << endl;
    cout << "2. Assign Driver to Next Order" << endl;
    cout << "3. Track Order" << endl;
    cout << "4. Complete Delivery" << endl;
    cout << "5. View Order Summary" << endl;
    cout << "6. Show Pending Queue" << endl;
    cout << "7. Exit" << endl;
    cout << "==========================================" << endl;
    cout << "Enter your choice (1-7): ";
}

int main() {
    DeliverySystem system;

    cout << "🚀 Online Food Delivery System Started!" << endl;
    cout << "Using: Queue (FIFO), Min-Heap (Drivers), Hash Map (Tracking)" << endl;

    int choice;

    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Clear newline

        switch (choice) {
            case 1: {
                string address;
                vector<string> items;

                cout << "\nEnter customer address: ";
                getline(cin, address);

                cout << "Enter number of items: ";
                int itemCount;
                cin >> itemCount;
                cin.ignore();

                for (int i = 0; i < itemCount; i++) {
                    cout << "Enter item " << (i + 1) << ": ";
                    string item;
                    getline(cin, item);
                    items.push_back(item);
                }

                system.placeOrder(address, items);
                break;
            }

            case 2:
                system.assignDriverToOrder();
                break;

            case 3: {
                string orderId;
                cout << "\nEnter Order ID to track: ";
                getline(cin, orderId);
                system.trackOrder(orderId);
                break;
            }

            case 4: {
                string orderId;
                cout << "\nEnter Order ID to mark as delivered: ";
                getline(cin, orderId);
                system.completeDelivery(orderId);
                break;
            }

            case 5:
                system.viewOrderSummary();
                break;

            case 6:
                system.showPendingQueue();
                break;

            case 7:
                cout << "\nThank you for using the Online Food Delivery System!" << endl;
                cout << "Exiting..." << endl;
                return 0;

            default:
                cout << "\n❌ Invalid choice. Please enter 1-7." << endl;
        }
    }

    return 0;
}
