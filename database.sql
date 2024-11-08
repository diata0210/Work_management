CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE
);

CREATE TABLE projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    owner_id INTEGER,
    FOREIGN KEY (owner_id) REFERENCES users(id)
);

CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER,
    assigned_to INTEGER,
    name TEXT NOT NULL,
    status TEXT NOT NULL,
    FOREIGN KEY (project_id) REFERENCES projects(id),
    FOREIGN KEY (assigned_to) REFERENCES users(id)
);

CREATE TABLE comments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id INTEGER,
    user_id INTEGER,
    content TEXT NOT NULL,
    FOREIGN KEY (task_id) REFERENCES tasks(id),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE attachments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id INTEGER,
    file_path TEXT NOT NULL,
    FOREIGN KEY (task_id) REFERENCES tasks(id)
);

CREATE TABLE messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER,
    user_id INTEGER,
    content TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (project_id) REFERENCES projects(id),
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Insert sample data
INSERT INTO users (username, password, email) VALUES ('user1', 'password1', 'user1@example.com');
INSERT INTO users (username, password, email) VALUES ('user2', 'password2', 'user2@example.com');

INSERT INTO projects (name, description, owner_id) VALUES ('Project 1', 'Description for Project 1', 1);
INSERT INTO projects (name, description, owner_id) VALUES ('Project 2', 'Description for Project 2', 2);

INSERT INTO tasks (project_id, assigned_to, name, status) VALUES (1, 1, 'Task 1', 'not started');
INSERT INTO tasks (project_id, assigned_to, name, status) VALUES (1, 2, 'Task 2', 'in progress');
INSERT INTO tasks (project_id, assigned_to, name, status) VALUES (2, 1, 'Task 3', 'completed');

INSERT INTO comments (task_id, user_id, content) VALUES (1, 1, 'Comment for Task 1');
INSERT INTO comments (task_id, user_id, content) VALUES (2, 2, 'Comment for Task 2');

INSERT INTO attachments (task_id, file_path) VALUES (1, '/path/to/file1.txt');
INSERT INTO attachments (task_id, file_path) VALUES (2, '/path/to/file2.txt');

INSERT INTO messages (project_id, user_id, content) VALUES (1, 1, 'Message for Project 1');
INSERT INTO messages (project_id, user_id, content) VALUES (2, 2, 'Message for Project 2');