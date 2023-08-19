CREATE SCHEMA IF NOT EXISTS just_post_schema;

CREATE TABLE IF NOT EXISTS just_post_schema.hello (
    name TEXT PRIMARY KEY,
    count INTEGER DEFAULT(1)
);

CREATE TABLE IF NOT EXISTS just_post_schema.users (
    user_id serial PRIMARY KEY,
    email text UNIQUE NOT NULL,
    passwd text NOT NULL
);

CREATE TABLE IF NOT EXISTS just_post_schema.post (
    post_id serial PRIMARY KEY,
    user_id int,
    FOREIGN KEY (user_id) REFERENCES just_post_schema.users(user_id),
    post_body text
);

CREATE TABLE IF NOT EXISTS just_post_schema.user_data (
    user_data_id serial PRIMARY KEY,
    user_id int,
    FOREIGN KEY (user_id) REFERENCES just_post_schema.users(user_id),
    user_name text DEFAULT 'Unknown',
    surname text DEFAULT 'Unknown',
    age int DEFAULT 0,
    gender varchar(20) DEFAULT 'Unknown',
    city text DEFAULT 'Unknown',
    date_of_birth date,
    user_status text DEFAULT 'offline',
    CONSTRAINT CHK_user_status CHECK (user_status = 'offline' OR user_status = 'online'),
    CONSTRAINT CHK_user_gender CHECK (gender = 'male' OR gender = 'female' OR gender = 'Unknown'),
    CONSTRAINT CHK_user_age CHECK (age >= 0)
);
