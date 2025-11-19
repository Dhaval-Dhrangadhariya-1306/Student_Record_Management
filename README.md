# 🎓 Student Record Management System (C Project)

A fully featured **console-based Student Record Management System** written in **C**, supporting  
adding, displaying, searching, updating, deleting, sorting, and statistical analysis of student performance.  
All data is stored in a **binary file (`records.dat`)**, ensuring permanent local storage.

---

## 🚀 Features

### 📝 Core Functionalities
- ➕ **Add New Student**
- 📃 **Display All Students**
- 🔍 **Search Student**  
  - By Roll Number  
  - By Partial Name  
- ✏️ **Update Student**  
  - Update name  
  - Update all subjects  
  - Update a single subject  
- ❌ **Delete Student**
- 🔢 **Sort Records**  
  - By Roll  
  - By Name  
  - By Percentage  
  - Ascending / Descending
- 📊 **Performance Statistics**  
  - Category breakdown (Excellent, Good, Average, etc.)
  - Highest & Lowest scoring students

---

## 🎨 UI Enhancements
The system includes **ANSI color support** for a clean and modern terminal UI:

- 🔵 Blue → Headings  
- 🟢 Green → Success  
- 🟡 Yellow → Warnings  
- 🔴 Red → Errors  
- 🔷 Cyan → Sections  

(Works on Linux / macOS and Windows 10+)

---

## 🗂 Data Storage
All student data is stored persistently using **binary serialization**:

