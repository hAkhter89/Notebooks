from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox, QTableWidgetItem, QCompleter
from PyQt6.QtCore import Qt
import sys
import traceback

class ManageCurrentBooksUI(QWidget):
    def __init__(self, admin_username="Admin"):
        super().__init__()
        
        try:
            # Load the UI file
            uic.loadUi('manage_current_books.ui', self)
            
            # Store admin username for reference
            self.admin_username = admin_username
            
            # Initialize selected book
            self.selected_book = None
            
            # Connect signals and slots
            self.connect_signals()
            
            # Initialize data from database
            self.initialize_data()
            
            # Show the window
            self.show()
            
        except Exception as e:
            print(f"DEBUG: Error initializing ManageCurrentBooksUI: {e}")
            print(f"DEBUG: Full traceback: {traceback.format_exc()}")
            raise
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        try:
            # Search functionality
            self.pushButtonSearch.clicked.connect(self.search_books)
            self.tableWidgetResults.itemSelectionChanged.connect(self.on_book_selected)
            
            # Copy management buttons
            self.pushButtonIncrement.clicked.connect(self.increment_copies)
            self.pushButtonDecrement.clicked.connect(self.decrement_copies)
            self.pushButtonUpdateCopies.clicked.connect(self.update_copies)
            
            # Add new book button
            self.pushButtonAddNewBook.clicked.connect(self.add_new_book)
            
            # Back button
            self.pushButtonBack.clicked.connect(self.go_back)
            
        except Exception as e:
            print(f"DEBUG: Error connecting signals: {e}")
            raise
    
    def initialize_data(self):
        """Initialize data from database"""
        try:
            # Import here to avoid circular imports
            from database import get_all_authors, get_all_genres
            
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
            
            # Clear selected book display
            self.update_selected_book_display()
            
        except Exception as e:
            print(f"DEBUG: Error initializing data: {e}")
            print(f"DEBUG: Full traceback: {traceback.format_exc()}")
            self.show_message("Error", f"Failed to initialize data: {str(e)}")
    
    def search_books(self):
        """Search books based on criteria from database"""
        try:
            from database import search_books, check_current_database
            
            book_name = self.lineEditBookName.text().strip()
            author_name = self.comboBoxAuthorName.currentText()
            genre = self.comboBoxGenre.currentText()
            
            # Checking database connection info
            # db_info = check_current_database()
            # print(f"DEBUG: Searching books in: {db_info}")
            
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
            print(f"DEBUG: Full traceback: {traceback.format_exc()}")
            self.show_message("Error", f"Search failed: {str(e)}")
    
    def display_books_in_table(self, books):
        """Display books in the results table"""
        try:
            self.tableWidgetResults.setRowCount(len(books))
            
            for row, book in enumerate(books):
                self.tableWidgetResults.setItem(row, 0, QTableWidgetItem(book["title"]))
                self.tableWidgetResults.setItem(row, 1, QTableWidgetItem(book["isbn"]))
                self.tableWidgetResults.setItem(row, 2, QTableWidgetItem(book["author"]))
                
                # --- NEW CHANGE: Column 3 is Genre ---
                self.tableWidgetResults.setItem(row, 3, QTableWidgetItem(book["genres"]))
                
                # --- CHANGED: Column 4 is Copies (Was 3) ---
                self.tableWidgetResults.setItem(row, 4, QTableWidgetItem(str(book["copies"])))
            
            self.tableWidgetResults.resizeColumnsToContents()
            
            if not books:
                self.tableWidgetResults.setRowCount(1)
                self.tableWidgetResults.setItem(0, 0, QTableWidgetItem("No books found matching your criteria"))
                # Clear columns 1-4
                for col in range(1, 5):
                    self.tableWidgetResults.setItem(0, col, QTableWidgetItem(""))
                    
        except Exception as e:
            print(f"DEBUG: Error displaying books: {e}")
            self.show_message("Error", f"Failed to display results: {str(e)}")
    
    def on_book_selected(self):
        """When a book is selected from the table"""
        try:
            current_row = self.tableWidgetResults.currentRow()
            if current_row >= 0:
                # Get items from specific columns
                title = self.tableWidgetResults.item(current_row, 0).text()
                isbn = self.tableWidgetResults.item(current_row, 1).text()
                author = self.tableWidgetResults.item(current_row, 2).text()
                
                # We can grab genre if we want, but it's not needed for the logic below
                # genre = self.tableWidgetResults.item(current_row, 3).text()
                
                # --- CRITICAL FIX: Get copies from Column 4 (Was 3) ---
                copies_text = self.tableWidgetResults.item(current_row, 4).text()
                
                # Safety check: ensure copies_text is a number
                if copies_text and copies_text.isdigit():
                    copies = int(copies_text)
                else:
                    copies = 0
                
                # Create book object from table data
                self.selected_book = {
                    "isbn": isbn,
                    "title": title,
                    "copies": copies,
                    "author": author
                }
                self.update_selected_book_display()
                
        except Exception as e:
            print(f"DEBUG: Error selecting book: {e}")
            # self.show_message("Error", f"Failed to select book: {str(e)}") # Optional: suppress error on empty rows
    
    def update_selected_book_display(self):
        """Update the display for the selected book"""
        try:
            if self.selected_book:
                self.labelSelectedBook.setText(f"Selected Book: {self.selected_book['title']}")
                self.labelCurrentCopies.setText(f"Current No. of Copies: {self.selected_book['copies']}")
                self.spinBoxSetCopies.setValue(self.selected_book['copies'])
            else:
                self.labelSelectedBook.setText("Selected Book: None")
                self.labelCurrentCopies.setText("Current No. of Copies: 0")
                self.spinBoxSetCopies.setValue(0)
                
        except Exception as e:
            print(f"DEBUG: Error updating display: {e}")
    
    def increment_copies(self):
        """Increment copies of selected book by 1"""
        try:
            from database import update_book_copies
            
            if self.selected_book:
                new_copies = self.selected_book['copies'] + 1
                success, message = update_book_copies(self.selected_book['isbn'], new_copies)
                
                if success:
                    # Update local data
                    self.selected_book['copies'] = new_copies
                    self.update_selected_book_display()
                    self.refresh_table()
                    self.show_message("Success", f"Incremented copies for '{self.selected_book['title']}' to {new_copies}")
                else:
                    self.show_message("Error", message)
            else:
                self.show_message("Error", "Please select a book first")
                
        except Exception as e:
            print(f"DEBUG: Error incrementing copies: {e}")
            self.show_message("Error", f"Failed to increment copies: {str(e)}")
    
    def decrement_copies(self):
        """Decrement copies of selected book by 1 (if > 0)"""
        try:
            from database import update_book_copies
            
            if self.selected_book:
                if self.selected_book['copies'] > 0:
                    new_copies = self.selected_book['copies'] - 1
                    success, message = update_book_copies(self.selected_book['isbn'], new_copies)
                    
                    if success:
                        # Update local data
                        self.selected_book['copies'] = new_copies
                        self.update_selected_book_display()
                        self.refresh_table()
                        self.show_message("Success", f"Decremented copies for '{self.selected_book['title']}' to {new_copies}")
                    else:
                        self.show_message("Error", message)
                else:
                    self.show_message("Error", "Cannot decrement below 0 copies")
            else:
                self.show_message("Error", "Please select a book first")
                
        except Exception as e:
            print(f"DEBUG: Error decrementing copies: {e}")
            self.show_message("Error", f"Failed to decrement copies: {str(e)}")
    
    def update_copies(self):
        """Update copies to the value in spin box"""
        try:
            from database import update_book_copies
            
            if self.selected_book:
                new_copies = self.spinBoxSetCopies.value()
                
                if new_copies < 0:
                    self.show_message("Error", "Number of copies cannot be negative")
                    return
                
                success, message = update_book_copies(self.selected_book['isbn'], new_copies)
                
                if success:
                    # Update local data
                    self.selected_book['copies'] = new_copies
                    self.update_selected_book_display()
                    self.refresh_table()
                    self.show_message("Success", f"Updated copies for '{self.selected_book['title']}' to {new_copies}")
                else:
                    self.show_message("Error", message)
            else:
                self.show_message("Error", "Please select a book first")
                
        except Exception as e:
            print(f"DEBUG: Error updating copies: {e}")
            self.show_message("Error", f"Failed to update copies: {str(e)}")
    
    def refresh_table(self):
        """Refresh the table to show updated data"""
        try:
            # Simply re-run the current search
            self.search_books()
        except Exception as e:
            print(f"DEBUG: Error refreshing table: {e}")
    
    def add_new_book(self):
        """Open add new book page"""
        try:
            from add_new_book import AddBookPage
            self.add_book_page = AddBookPage(self.admin_username)
            self.add_book_page.show()
            self.hide()
        except Exception as e:
            print(f"DEBUG: Error opening Add Book page: {e}")
            self.show_message("Error", f"Failed to open Add Book page: {str(e)}")
    
    def go_back(self):
        """Return to admin dashboard"""
        try:
            from admin_dashboard import AdminDashboardUI
            self.admin_dashboard = AdminDashboardUI(self.admin_username)
            self.admin_dashboard.show()
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