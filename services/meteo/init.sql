CREATE DATABASE IF NOT EXISTS `meteo`;
GRANT USAGE ON *.* TO 'meteo'@'%' IDENTIFIED BY 'DhBr1lONLzMetf7mhROc';
GRANT ALL PRIVILEGES ON `meteo`.* TO 'meteo'@'%';

DROP TABLE IF EXISTS `data`;
CREATE TABLE IF NOT EXISTS `data` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `lat` decimal(10,0) NOT NULL,
  `long` decimal(10,0) NOT NULL,
  `what` enum('airport','city') CHARACTER SET latin1 NOT NULL,
  `place` varchar(255) CHARACTER SET latin1 NOT NULL,
  `metar` varchar(250) CHARACTER SET latin1 NOT NULL,
  `comment` varchar(250) CHARACTER SET latin1 NOT NULL,
  `hash` varchar(32) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `place` (`place`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `filters`;
CREATE TABLE IF NOT EXISTS `filters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `city` varchar(255) NOT NULL,
  `theme` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;
