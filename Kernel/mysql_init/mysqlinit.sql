-- MySQL dump 10.13  Distrib 5.1.73, for redhat-linux-gnu (x86_64)
--
-- Host: localhost    Database: DmpSWDBTest
-- ------------------------------------------------------
-- Server version	5.1.73

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `exp_data`
--

DROP TABLE IF EXISTS `index_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `index_table` (
  `exp_id` int(32) NOT NULL AUTO_INCREMENT,
  `StartTime` varchar(32) NOT NULL,
  `StopTime` varchar(32) NOT NULL,
  `FileName` varchar(32),
  `Temperature` double DEFAULT NULL,
  `Detector` int(2) DEFAULT NULL,
  `Type` int(2) NOT NULL,
  PRIMARY KEY (`exp_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `exp_data`
--

LOCK TABLES `index_table` WRITE;
UNLOCK TABLES;

--
-- Table structure for table `set_data`
--

DROP TABLE IF EXISTS `pedestal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pedestal` (
  `log_id` int(64) NOT NULL AUTO_INCREMENT,
  `global_id` int(32) NOT NULL,
  `mean` double NOT NULL,
  `sigma` double NOT NULL,
  `NDF` double NOT NULL,
  `entries` double NOT NULL,
  `fit_status` int(32) NOT NULL,
  PRIMARY KEY (`log_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `set_data`
--

LOCK TABLES `pedestal` WRITE;
UNLOCK TABLES;

