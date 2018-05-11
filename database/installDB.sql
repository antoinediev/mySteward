DROP USER 'root'@'localhost';
CREATE USER 'root'@'localhost' IDENTIFIED BY 'raspberry';
GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost';
