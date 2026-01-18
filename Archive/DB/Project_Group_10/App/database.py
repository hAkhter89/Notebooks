import pyodbc
from datetime import datetime, timedelta
# Database connection configuration
server = r'localhost\SQLEXPRESS'
database = 'LibraryDB'
use_windows_authentication = True
username = ''
password = ''


if use_windows_authentication:
 connection_string = f'DRIVER={{ODBC Driver 17 for SQL Server}};SERVER={server};DATABASE={database};Trusted_Connection=yes;'
else:
 connection_string = f'DRIVER={{ODBC Driver 17 for SQL Server}};SERVER={server};DATABASE={database};UID={username};PWD={password}'

class DatabaseConnection:
    def __init__(self):
        self.connection_string = connection_string
    
    def get_connection(self):
        """Get database connection"""
        try:
            conn = pyodbc.connect(self.connection_string)
            return conn
        except pyodbc.Error as e:
            print(f"Database connection error: {e}")
            return None
    
    def execute_query(self, query, params=None):
        """Execute SELECT queries and return results"""
        conn = self.get_connection()
        if conn:
            try:
                cursor = conn.cursor()
                if params:
                    cursor.execute(query, params)
                else:
                    cursor.execute(query)
                results = cursor.fetchall()
                conn.close()
                return results
            except pyodbc.Error as e:
                print(f"Query execution error: {e}")
                return None
        return None
    
    def execute_update(self, query, params=None):
        """Execute INSERT, UPDATE, DELETE queries"""
        conn = self.get_connection()
        if conn:
            try:
                cursor = conn.cursor()
                if params:
                    cursor.execute(query, params)
                else:
                    cursor.execute(query)
                conn.commit()
                conn.close()
                return True
            except pyodbc.Error as e:
                print(f"Update execution error: {e}")
                conn.rollback()
                return False
        return False
    
    def execute_scalar(self, query, params=None, auto_commit=True):
        """Execute query and return single value (first column of first row)"""
        conn = self.get_connection()
        if conn:
            try:
                cursor = conn.cursor()
                if params:
                    cursor.execute(query, params)
                else:
                    cursor.execute(query)
                result = cursor.fetchone()
                
                # Only commit if auto_commit is True AND it's an INSERT/UPDATE/DELETE
                if auto_commit and query.strip().upper().startswith(('INSERT', 'UPDATE', 'DELETE')):
                    conn.commit()
                # For SELECT queries, we don't need commit
                    
                conn.close()
                return result[0] if result else None
            except pyodbc.Error as e:
                print(f"Scalar execution error: {e}")
                if auto_commit:
                    conn.rollback()
                return None
        return None
    
    def execute_insert_with_output(self, query, params=None):
        """Execute INSERT query with OUTPUT clause and return the output value"""
        conn = self.get_connection()
        if conn:
            try:
                cursor = conn.cursor()
                if params:
                    cursor.execute(query, params)
                else:
                    cursor.execute(query)
                result = cursor.fetchone()
                # Explicit commit for INSERT operations
                conn.commit()  
                conn.close()
                return result[0] if result else None
            except pyodbc.Error as e:
                print(f"Insert with output error: {e}")
                conn.rollback()
                return None
        return None

# Global database instance for easy access
db = DatabaseConnection()

def register_user(name, email, password, role):
    """
    Register a new user (Member or Librarian) in the database
    Returns: (success, message, user_id)
    """
    try:
        # Check if email already exists in both tables (case-sensitive)
        check_query = """
        SELECT 'Member' as role FROM Member WHERE Email COLLATE SQL_Latin1_General_CP1_CS_AS = ?
        UNION ALL
        SELECT 'Librarian' as role FROM Librarian WHERE Email COLLATE SQL_Latin1_General_CP1_CS_AS = ?
        """
        existing = db.execute_query(check_query, (email, email))
        
        if existing:
            return False, "Email already exists. Please use a different email.", None
        
        # Insert based on role
        if role == "Member":
            insert_query = "INSERT INTO Member (Name, Email, Password) OUTPUT INSERTED.Member_id VALUES (?, ?, ?)"
            table_name = "Member"
        else:  # Librarian
            insert_query = "INSERT INTO Librarian (Name, Email, Password) OUTPUT INSERTED.Librarian_id VALUES (?, ?, ?)"
            table_name = "Librarian"
        
        # Execute insert and get the auto-generated ID
        user_id = db.execute_insert_with_output(insert_query, (name, email, password))
        
        # Immediately check if the record was actually inserted
        if user_id:
            verify_query = f"SELECT COUNT(*) FROM {table_name} WHERE {table_name.lower()}_id = ?"
            verify_count = db.execute_scalar(verify_query, (user_id,))
        
        if user_id:
            return True, f"Registration successful! Welcome {name}.", user_id
        else:
            return False, "Registration failed. Please try again.", None
            
    except Exception as e:
        print(f"DEBUG: Exception occurred: {str(e)}")
        import traceback
        print(f"DEBUG: Full traceback: {traceback.format_exc()}")
        return False, f"Database error: {str(e)}", None

def authenticate_user(username, email, password):
    """
    Authenticate a user (Member or Librarian) with username, email, and password (case-sensitive)
    Returns: (success, message, role, user_id)
    """
    try:
        # Check both Member and Librarian tables with all three fields (case-sensitive)
        auth_query = """
        SELECT 'Member' as role, Member_id as user_id FROM Member 
        WHERE Name COLLATE SQL_Latin1_General_CP1_CS_AS = ? 
        AND Email COLLATE SQL_Latin1_General_CP1_CS_AS = ? 
        AND Password COLLATE SQL_Latin1_General_CP1_CS_AS = ?
        UNION ALL
        SELECT 'Librarian' as role, Librarian_id as user_id FROM Librarian 
        WHERE Name COLLATE SQL_Latin1_General_CP1_CS_AS = ? 
        AND Email COLLATE SQL_Latin1_General_CP1_CS_AS = ? 
        AND Password COLLATE SQL_Latin1_General_CP1_CS_AS = ?
        """
        
        result = db.execute_query(auth_query, (username, email, password, username, email, password))
        
        if result:
            role = result[0][0]  # 'Member' or 'Librarian'
            user_id = result[0][1]  # The user ID
            return True, f"Welcome back, {username}!", role, user_id
        else:
            return False, "Invalid name, email or password", None, None
            
    except Exception as e:
        print(f"DEBUG: Authentication error: {str(e)}")
        return False, f"Authentication error: {str(e)}", None, None

def get_user_email(username, role):
    """
    Get user's email for display purposes
    """
    try:
        if role == "Member":
            query = "SELECT Email FROM Member WHERE Name = ?"
        else:  # Librarian
            query = "SELECT Email FROM Librarian WHERE Name = ?"
        
        email = db.execute_scalar(query, (username,))
        return email
    except Exception as e:
        print(f"DEBUG: Error getting user email: {e}")
        return f"{username}@example.com"  # Fallback
    
def check_isbn_exists(isbn):
    """
    Check if a book with the given ISBN already exists
    Returns: True if exists, False otherwise
    """
    conn = db.get_connection()
    if not conn:
        return True  # Assume exists on connection failure
    
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT COUNT(*) FROM Book WHERE ISBN = ?", (isbn,))
        result = cursor.fetchone()
        count = result[0] if result else 0
        conn.close()
        return count > 0
    except Exception as e:
        print(f"DEBUG: Error checking ISBN: {e}")
        conn.close()
        return True  # Assume exists to prevent duplicates on error


def add_new_book_to_database(book_data):
    """
    Add a new book to the database with authors and genres - SIMPLIFIED VERSION
    """
    try:
        # 1. First check if ISBN already exists
        if check_isbn_exists(book_data['isbn']):
            return False, f"Book with ISBN {book_data['isbn']} already exists"
        
        # 2. Insert into Book table using execute_update (which has its own commit)
        insert_book_query = "INSERT INTO Book (ISBN, Name, No_of_copies) VALUES (?, ?, ?)"
        
        book_success = db.execute_update(insert_book_query, (book_data['isbn'], book_data['title'], book_data['copies']))
        if not book_success:
            return False, "Failed to insert book"
        
        # 3. Insert authors into Book_Author table
        for i, author in enumerate(book_data['authors']):
            insert_author_query = "INSERT INTO Book_Author (ISBN, Author) VALUES (?, ?)"
            author_success = db.execute_update(insert_author_query, (book_data['isbn'], author))
            if not author_success:
                return False, f"Failed to insert author: {author}"
        
        # 4. Insert genres into Book_Genre table
        for i, genre in enumerate(book_data['genres']):
            insert_genre_query = "INSERT INTO Book_Genre (ISBN, Genre) VALUES (?, ?)"
            genre_success = db.execute_update(insert_genre_query, (book_data['isbn'], genre))
            if not genre_success:
                return False, f"Failed to insert genre: {genre}"
        
        return True, f"Book '{book_data['title']}' added successfully with {len(book_data['authors'])} authors and {len(book_data['genres'])} genres"
        
    except Exception as e:
        print(f"DEBUG: Error adding book: {e}")
        import traceback
        print(f"DEBUG: Full traceback: {traceback.format_exc()}")
        return False, f"Error: {str(e)}"

def get_all_authors():
    """
    Get all distinct authors from the database for the dropdown
    Returns: list of author names
    """
    try:
        query = "SELECT DISTINCT Author FROM Book_Author ORDER BY Author"
        results = db.execute_query(query)
        return [row[0] for row in results] if results else []
    except Exception as e:
        print(f"DEBUG: Error getting authors: {e}")
        return []

def add_new_author_to_database(author_name):
    """
    Add a new author to the authors list (for the +Add author functionality)
    Note: This doesn't insert into Book_Author yet, just makes the author available
    """
    try:
        # Check if author already exists in Book_Author
        query = "SELECT COUNT(*) FROM Book_Author WHERE Author COLLATE SQL_Latin1_General_CP1_CS_AS = ?"
        count = db.execute_scalar(query, (author_name,), auto_commit=False)  # <-- Add auto_commit=False
        if count and count > 0:
            return False, f"Author '{author_name}' already exists in the system"
        
        # For now, we'll just return success since the author will be added when the book is saved
        return True, f"Author '{author_name}' will be added when you save the book"
    except Exception as e:
        print(f"DEBUG: Error checking author: {e}")
        return False, f"Error: {str(e)}"
    
def test_database_connection():
    """Test if database connection and basic operations work"""
    try:
        print("DEBUG: Testing database connection...")
        
        # Test connection
        conn = db.get_connection()
        if not conn:
            print("DEBUG: Database connection FAILED")
            return False
        
        print("DEBUG: Database connection SUCCESS")
        
        # Test simple query
        cursor = conn.cursor()
        cursor.execute("SELECT COUNT(*) FROM Book")
        result = cursor.fetchone()
        print(f"DEBUG: Current books in database: {result[0] if result else 'Unknown'}")
        
        conn.close()
        return True
        
    except Exception as e:
        print(f"DEBUG: Database test failed: {e}")
        return False
    
def verify_book_insertion(isbn):
    """Verify that a book was inserted correctly using the same connection method"""
    try:
        conn = db.get_connection()
        if not conn:
            return False
        
        cursor = conn.cursor()
        
        # Set the same isolation level
        cursor.execute("SET TRANSACTION ISOLATION LEVEL READ COMMITTED")
        
        # Check Book table
        cursor.execute("SELECT COUNT(*) FROM Book WHERE ISBN = ?", (isbn,))
        book_count = cursor.fetchone()[0]
        
        # Check Book_Author table
        cursor.execute("SELECT COUNT(*) FROM Book_Author WHERE ISBN = ?", (isbn,))
        author_count = cursor.fetchone()[0]
        
        # Check Book_Genre table
        cursor.execute("SELECT COUNT(*) FROM Book_Genre WHERE ISBN = ?", (isbn,))
        genre_count = cursor.fetchone()[0]
        
        conn.close()
        
        return book_count > 0 and author_count > 0 and genre_count > 0
        
    except Exception as e:
        print(f"DEBUG: Verification error: {e}")
        if 'conn' in locals():
            conn.close()
        return False
    
def check_current_database():
    """Check which database we're actually connected to"""
    try:
        conn = db.get_connection()
        if not conn:
            return "No connection"
        
        cursor = conn.cursor()
        cursor.execute("SELECT DB_NAME() AS CurrentDatabase")
        result = cursor.fetchone()
        current_db = result[0] if result else "Unknown"
        
        cursor.execute("SELECT @@SERVERNAME AS ServerName")
        result = cursor.fetchone()
        server_name = result[0] if result else "Unknown"
        
        conn.close()
        
        return f"Connected to: Database='{current_db}', Server='{server_name}'"
        
    except Exception as e:
        return f"Error: {str(e)}"

def search_books(book_name=None, author_name=None, genre=None):
    """
    Search books based on multiple criteria
    Returns: list of books with ISBN, Name, Authors (comma-separated), No_of_copies
    """
    try:
        # Build the query based on provided criteria
        query = """
        SELECT 
            b.ISBN, 
            b.Name AS BookName, 
            b.No_of_copies,
            STUFF((
                SELECT ', ' + ba2.Author 
                FROM Book_Author ba2 
                WHERE ba2.ISBN = b.ISBN 
                FOR XML PATH(''), TYPE).value('.', 'NVARCHAR(MAX)'), 1, 2, ''
            ) AS Authors,
            STUFF((
                SELECT ', ' + bg2.Genre 
                FROM Book_Genre bg2 
                WHERE bg2.ISBN = b.ISBN 
                FOR XML PATH(''), TYPE).value('.', 'NVARCHAR(MAX)'), 1, 2, ''
            ) AS Genres
        FROM Book b
        WHERE 1=1
        """
        params = []
        
        # Only add filters if they are provided (not None)
        if book_name is not None:  # This allows empty string to mean "no filter"
            query += " AND b.Name LIKE ?"
            params.append(f'%{book_name}%')
        
        if author_name is not None:  # Only filter if explicitly provided
            query += " AND EXISTS (SELECT 1 FROM Book_Author ba WHERE ba.ISBN = b.ISBN AND ba.Author LIKE ?)"
            params.append(f'%{author_name}%')
        
        if genre is not None:  # Only filter if explicitly provided  
            query += " AND EXISTS (SELECT 1 FROM Book_Genre bg WHERE bg.ISBN = b.ISBN AND bg.Genre = ?)"
            params.append(genre)
        
        query += " ORDER BY b.Name"
        
        results = db.execute_query(query, params)
        
        books = []
        if results:
            for row in results:
                books.append({
                    "isbn": row[0],
                    "title": row[1],
                    "copies": row[2],
                    "author": row[3] if row[3] else "Unknown Author",
                    "genres": row[4] if row[4] else "Unknown Genre"
                })
        
        return books
        
    except Exception as e:
        print(f"DEBUG: Error searching books: {e}")
        import traceback
        print(f"DEBUG: Full traceback: {traceback.format_exc()}")
        return []
    
    
def get_all_genres():
    """
    Get all distinct genres from the database
    Returns: list of genre names
    """
    try:
        query = "SELECT DISTINCT Genre FROM Book_Genre ORDER BY Genre"
        results = db.execute_query(query)
        return [row[0] for row in results] if results else []
    except Exception as e:
        print(f"DEBUG: Error getting genres: {e}")
        return []

def update_book_copies(isbn, new_copies):
    """
    Update the number of copies for a book
    Returns: (success, message)
    """
    try:
        query = "UPDATE Book SET No_of_copies = ? WHERE ISBN = ?"
        success = db.execute_update(query, (new_copies, isbn))
        
        if success:
            return True, f"Updated copies to {new_copies}"
        else:
            return False, "Failed to update copies"
            
    except Exception as e:
        print(f"DEBUG: Error updating copies: {e}")
        return False, f"Error: {str(e)}"

def get_book_details(isbn):
    """
    Get detailed information about a specific book
    Returns: book data or None if not found
    """
    try:
        query = """
        SELECT 
            b.ISBN, 
            b.Name, 
            b.No_of_copies
        FROM Book b
        WHERE b.ISBN = ?
        """
        results = db.execute_query(query, (isbn,))
        
        if results:
            row = results[0]
            return {
                "isbn": row[0],
                "title": row[1],
                "copies": row[2]
            }
        return None
        
    except Exception as e:
        print(f"DEBUG: Error getting book details: {e}")
        return None