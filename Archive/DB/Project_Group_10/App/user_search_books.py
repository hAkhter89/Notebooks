from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox, QTableWidgetItem, QCompleter
from PyQt6.QtCore import Qt
import sys
from database import search_books, get_all_authors, get_all_genres

class UserSearchBooksPage(QWidget):
    def __init__(self, username="User"):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('user_search_books.ui', self)
        
        # Store username for reference
        self.username = username
        
        # Update welcome message
        self.welcomeLabel.setText(f"Welcome, {username}!")
        
        # Connect signals and slots
        self.connect_signals()
        
        # Initialize data from database
        self.initialize_data()
        
        # Clear the table initially - show no results
        self.clear_results_table()
        
        # Show the window
        self.show()
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        self.pushButtonSearch.clicked.connect(self.search_books)
        self.pushButtonBack.clicked.connect(self.go_back)
    
    def initialize_data(self):
        """Initialize data from database"""
        try:
            # Populate author dropdown
            authors = get_all_authors()
            self.comboBoxAuthorName.clear()
            self.comboBoxAuthorName.addItem("Any")
            self.comboBoxAuthorName.addItems(sorted(authors))
            
            # Populate genre dropdown
            genres = get_all_genres()
            self.comboBoxGenre.clear()
            self.comboBoxGenre.addItem("Any")
            self.comboBoxGenre.addItems(sorted(genres))
            
            # Set up autocomplete for author dropdown
            self.setup_autocomplete()
            
        except Exception as e:
            print(f"DEBUG: Error initializing data: {e}")
            self.show_message("Error", f"Failed to initialize data: {str(e)}")
    
    def setup_autocomplete(self):
        """Set up autocomplete functionality for author combobox"""
        try:
            author_completer = QCompleter([self.comboBoxAuthorName.itemText(i) for i in range(1, self.comboBoxAuthorName.count())])
            author_completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
            author_completer.setFilterMode(Qt.MatchFlag.MatchContains)
            self.comboBoxAuthorName.setCompleter(author_completer)
        except Exception as e:
            print(f"DEBUG: Error setting up autocomplete: {e}")
    
    def clear_results_table(self):
        """Clear the results table and show a message"""
        self.tableWidgetResults.setRowCount(1)
        self.tableWidgetResults.setItem(0, 0, QTableWidgetItem("Enter search criteria and click 'Search' to find books"))
        for col in range(1, 4):
            self.tableWidgetResults.setItem(0, col, QTableWidgetItem(""))
        
        # Update the results label
        self.labelResults.setText("Search Results")
    
    def search_books(self):
        """Search books based on criteria from database"""
        try:
            book_name = self.lineEditBookName.text().strip()
            author_name = self.comboBoxAuthorName.currentText()
            genre = self.comboBoxGenre.currentText()
            
            # If all fields are empty/Any, show ALL books
            if not book_name and author_name == "Any" and genre == "Any":
                books = search_books(book_name=None, author_name=None, genre=None)
            else:
                # Search with the provided criteria
                books = search_books(
                    book_name=book_name if book_name else None,
                    author_name=author_name if author_name != "Any" else None,
                    genre=genre if genre != "Any" else None
                )
            
            self.display_books_in_table(books)
            
        except Exception as e:
            print(f"DEBUG: Error in search_books: {e}")
            self.show_message("Error", f"Search failed: {str(e)}")
    
    def display_books_in_table(self, books):
        """Display books in the results table with availability status"""
        try:
            if not books:
                # Update row count and clear columns
                self.tableWidgetResults.setRowCount(1)
                self.tableWidgetResults.setItem(0, 0, QTableWidgetItem("No books found matching your criteria"))
                # Clear remaining columns (Now 1 through 4)
                for col in range(1, 5): 
                    self.tableWidgetResults.setItem(0, col, QTableWidgetItem(""))
                self.labelResults.setText("Search Results (0 books found)")
            else:
                # Display found books
                self.tableWidgetResults.setRowCount(len(books))
                
                for row, book in enumerate(books):
                    # Column 0: Title
                    self.tableWidgetResults.setItem(row, 0, QTableWidgetItem(book["title"]))
                    # Column 1: ISBN
                    self.tableWidgetResults.setItem(row, 1, QTableWidgetItem(book["isbn"]))
                    # Column 2: Author
                    self.tableWidgetResults.setItem(row, 2, QTableWidgetItem(book["author"]))
                    
                    # --- NEW CHANGE: Column 3: Genre ---
                    self.tableWidgetResults.setItem(row, 3, QTableWidgetItem(book["genres"]))
                    
                    # --- CHANGED: Column 4: Copies (Was 3) ---
                    copies = book["copies"]
                    if copies > 0:
                        copies_text = f"{copies} (Available)"
                        copies_item = QTableWidgetItem(copies_text)
                        copies_item.setForeground(Qt.GlobalColor.darkGreen)  
                    else:
                        copies_text = f"{copies} (Out of Stock)"
                        copies_item = QTableWidgetItem(copies_text)
                        copies_item.setForeground(Qt.GlobalColor.red)  
                    
                    self.tableWidgetResults.setItem(row, 4, copies_item) # Set to column 4
                
                # Resize columns
                self.tableWidgetResults.resizeColumnsToContents()
                self.labelResults.setText(f"Search Results ({len(books)} books found)")
                
        except Exception as e:
            print(f"DEBUG: Error displaying books: {e}")
            self.show_message("Error", f"Failed to display results: {str(e)}")
    
    def go_back(self):
        """Return to user dashboard"""
        try:
            from user_dashboard import UserDashboardUI
            self.dashboard = UserDashboardUI(self.username)
            self.dashboard.show()
            self.hide()
        except Exception as e:
            print(f"DEBUG: Error going back to dashboard: {e}")
            self.show_message("Error", f"Failed to return to dashboard: {str(e)}")
    
    def show_message(self, title, message):
        """Show message box"""
        try:
            msg = QMessageBox()
            msg.setWindowTitle(title)
            msg.setText(message)
            msg.exec()
        except Exception as e:
            print(f"DEBUG: Error showing message: {e}")