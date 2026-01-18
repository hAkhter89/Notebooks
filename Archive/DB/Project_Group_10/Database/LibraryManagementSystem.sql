--Create database manually and then run this code to create table
CREATE TABLE Member (
    Member_id INT IDENTITY(1,1) PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    Email VARCHAR(100) UNIQUE NOT NULL,
    Password VARCHAR(100) NOT NULL
);

CREATE TABLE Librarian (
    Librarian_id INT IDENTITY(1,1) PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    Email VARCHAR(100) UNIQUE NOT NULL,
    Password VARCHAR(100) NOT NULL
);

CREATE TABLE Book (
    ISBN VARCHAR(20) PRIMARY KEY,
    Name VARCHAR(255) NOT NULL,
    No_of_copies INT NOT NULL
);

CREATE TABLE Borrow (
    Member_Id INT,
    ISBN VARCHAR(20),
    Borrow_Date DATETIME NOT NULL,
    Return_Due_Date DATETIME NOT NULL,
    Return_Date DATETIME NULL,
    PRIMARY KEY (Member_Id, ISBN, Borrow_Date),
    FOREIGN KEY (Member_Id) REFERENCES Member(Member_id),
    FOREIGN KEY (ISBN) REFERENCES Book(ISBN)
);

CREATE TABLE Check_out (
    Librarian_Id INT,
    ISBN VARCHAR(20),
    Check_out_date DATETIME,
    PRIMARY KEY (Librarian_Id, ISBN, Check_out_date),
    FOREIGN KEY (Librarian_Id) REFERENCES Librarian(Librarian_id),
    FOREIGN KEY (ISBN) REFERENCES Book(ISBN)
);

CREATE TABLE Check_in (
    Librarian_Id INT,
    ISBN VARCHAR(20),
    Check_in_date DATETIME,
    PRIMARY KEY (Librarian_Id, ISBN, Check_in_date),
    FOREIGN KEY (Librarian_Id) REFERENCES Librarian(Librarian_id),
    FOREIGN KEY (ISBN) REFERENCES Book(ISBN)
);

CREATE TABLE Book_Author (
    ISBN VARCHAR(20),
    Author VARCHAR(100),
    PRIMARY KEY (ISBN, Author),
    FOREIGN KEY (ISBN) REFERENCES Book(ISBN)
);

CREATE TABLE Book_Genre (
    ISBN VARCHAR(20),
    Genre VARCHAR(50),
    PRIMARY KEY (ISBN, Genre),
    FOREIGN KEY (ISBN) REFERENCES Book(ISBN)
);