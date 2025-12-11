# Online Food Delivery Order System

## Project Overview
A C++ implementation of an online food delivery system that efficiently manages orders and drivers using three core data structures:
- **Queue** (FIFO) for order management
- **Min-Heap** for optimal driver assignment  
- **Hash Map** for real-time order tracking

## Data Structures Used
| Data Structure | Purpose | Implementation |
|----------------|---------|----------------|
| **Queue** | Store incoming orders in FIFO order | `std::queue<Order*>` |
| **Min-Heap** | Store drivers sorted by availability | `std::priority_queue<Driver>` |
| **Hash Map** | Map order ID → order details | `std::unordered_map<string, Order*>` |

## Features
- **Place Order** - Add new customer orders to queue
- **Assign Driver** - Automatically assign available driver using min-heap
- **Track Order** - Real-time status tracking using hash map
- **Complete Delivery** - Update order status and driver availability
- **View Summary** - Display all active, pending, and completed orders

## Project Structure


## How to Compile & Run

### **Method 1: Using Code::Blocks**
1. Open Code::Blocks IDE
2. Create new Console Application project
3. Copy code from `delivery_system.cpp`
4. Build → Run (F9)

==========================================
   ONLINE FOOD DELIVERY SYSTEM
==========================================
1. Place New Order
2. Assign Driver to Next Order
3. Track Order
4. Complete Delivery
5. View Order Summary
6. Show Pending Queue
7. Exit
Enter your choice (1-7): 1

Enter customer address: 123 Main Street
Enter number of items: 2
Enter item 1: Pizza
Enter item 2: Coke

   Order placed successfully!
   Order ID: ORD1
   Address: 123 Main Street
   Items: Pizza Coke
   Status: pending
