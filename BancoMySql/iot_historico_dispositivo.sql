-- MySQL dump 10.13  Distrib 8.0.38, for Win64 (x86_64)
--
-- Host: 192.168.1.198    Database: iot
-- ------------------------------------------------------
-- Server version	9.0.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `historico_dispositivo`
--

DROP TABLE IF EXISTS `historico_dispositivo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `historico_dispositivo` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT 'Primary Key',
  `iddisp` int NOT NULL,
  `dateregister` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` int NOT NULL,
  PRIMARY KEY (`id`),
  KEY `iddisp` (`iddisp`),
  CONSTRAINT `historico_dispositivo_ibfk_1` FOREIGN KEY (`iddisp`) REFERENCES `dispositivo` (`iddisp`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `historico_dispositivo`
--

LOCK TABLES `historico_dispositivo` WRITE;
/*!40000 ALTER TABLE `historico_dispositivo` DISABLE KEYS */;
INSERT INTO `historico_dispositivo` VALUES (50,4,'2025-01-12 05:59:49',0),(51,4,'2025-01-12 05:59:56',1),(52,4,'2025-01-12 06:00:04',0),(53,3,'2025-01-12 06:01:45',1),(54,3,'2025-01-12 06:01:47',0),(55,3,'2025-01-12 06:06:18',1),(56,3,'2025-01-12 06:06:20',0),(57,3,'2025-01-12 06:07:42',1),(58,3,'2025-01-12 06:07:44',0),(59,3,'2025-01-12 06:10:56',1),(60,3,'2025-01-12 06:10:58',0),(61,3,'2025-01-12 06:12:26',1),(62,3,'2025-01-12 06:12:27',0),(63,3,'2025-01-12 06:14:00',1),(64,3,'2025-01-12 06:14:02',0),(65,3,'2025-01-12 06:17:26',1),(66,3,'2025-01-12 06:17:28',0),(67,3,'2025-01-12 06:18:53',1),(68,3,'2025-01-12 06:18:55',0),(69,3,'2025-01-12 06:20:02',1),(70,4,'2025-01-12 06:20:11',1),(71,3,'2025-01-12 06:20:21',0),(72,3,'2025-01-12 06:20:32',1),(73,4,'2025-01-12 06:20:45',0),(74,3,'2025-01-12 06:20:55',0),(75,3,'2025-01-12 06:21:26',1),(76,3,'2025-01-12 06:21:28',0),(77,3,'2025-01-12 06:22:06',1),(78,3,'2025-01-12 06:22:22',0),(79,3,'2025-01-12 06:22:27',1),(80,3,'2025-01-12 06:22:36',0),(81,3,'2025-01-12 06:24:33',1),(82,3,'2025-01-12 06:24:36',0),(83,3,'2025-01-12 06:28:09',1),(84,3,'2025-01-12 06:28:11',0),(85,3,'2025-01-12 06:29:24',1),(86,3,'2025-01-12 06:29:26',0),(87,3,'2025-01-12 06:30:10',1),(88,3,'2025-01-12 06:30:12',0),(89,3,'2025-01-12 06:30:24',1),(90,3,'2025-01-12 06:30:26',0),(91,3,'2025-01-12 06:33:07',1),(92,3,'2025-01-12 06:33:09',0),(93,3,'2025-01-12 06:33:23',1),(94,3,'2025-01-12 06:33:25',0),(95,3,'2025-01-12 06:42:15',1),(96,3,'2025-01-12 06:42:17',0),(97,3,'2025-01-12 06:42:52',1),(98,4,'2025-01-12 06:44:37',1),(99,4,'2025-01-12 06:44:41',0),(100,4,'2025-01-12 06:45:34',1),(101,4,'2025-01-12 06:45:38',0),(102,2,'2025-01-12 06:46:17',1),(103,2,'2025-01-12 06:46:29',0),(104,2,'2025-01-12 06:46:38',1),(105,2,'2025-01-12 06:46:38',0),(106,3,'2025-01-12 06:46:38',1),(107,3,'2025-01-12 06:46:40',0),(108,2,'2025-01-12 06:46:45',1),(109,2,'2025-01-12 06:46:47',0),(110,1,'2025-01-12 06:46:49',1),(111,4,'2025-01-12 06:47:10',1),(112,1,'2025-01-12 06:47:30',0),(113,3,'2025-01-12 06:47:44',1),(114,3,'2025-01-12 06:47:46',0),(115,3,'2025-01-12 07:01:57',1),(116,3,'2025-01-12 07:01:59',0),(117,3,'2025-01-12 07:02:22',1),(118,3,'2025-01-12 07:02:24',0),(119,3,'2025-01-12 07:05:08',1),(120,3,'2025-01-12 07:05:09',0),(121,3,'2025-01-12 07:06:44',1),(122,3,'2025-01-12 07:06:55',0),(123,3,'2025-01-12 07:07:15',1),(124,3,'2025-01-12 07:07:21',0),(125,3,'2025-01-12 07:07:32',1),(126,4,'2025-01-12 07:07:54',1),(127,3,'2025-01-12 07:07:58',0),(128,3,'2025-01-12 07:08:00',1),(129,4,'2025-01-12 07:08:01',0),(130,3,'2025-01-12 07:08:04',0),(131,3,'2025-01-12 07:08:07',1),(132,3,'2025-01-12 07:08:09',0),(133,2,'2025-01-12 07:09:26',1),(134,2,'2025-01-12 07:09:38',0),(135,1,'2025-01-12 07:09:39',1),(136,1,'2025-01-12 07:10:21',0),(137,3,'2025-01-12 07:10:24',1),(138,3,'2025-01-12 07:10:26',0),(139,3,'2025-01-12 07:10:42',1),(140,3,'2025-01-12 07:10:44',0),(141,4,'2025-01-12 07:19:50',1),(142,1,'2025-01-12 07:21:06',1),(143,1,'2025-01-12 07:21:11',0),(144,2,'2025-01-12 07:21:13',1),(145,2,'2025-01-12 07:21:23',0),(146,1,'2025-01-12 07:21:25',1),(147,4,'2025-01-12 07:21:26',0),(148,1,'2025-01-12 07:21:58',0),(149,1,'2025-01-12 07:21:58',0),(150,3,'2025-01-12 07:21:59',1),(151,3,'2025-01-12 07:22:00',0),(152,3,'2025-01-12 07:22:09',1),(153,3,'2025-01-12 07:22:15',0),(154,3,'2025-01-12 07:27:39',1),(155,3,'2025-01-12 07:27:41',0),(156,3,'2025-01-12 07:30:48',1),(157,3,'2025-01-12 07:30:50',0),(158,3,'2025-01-12 07:31:44',1),(159,3,'2025-01-12 07:31:49',0),(160,3,'2025-01-12 07:32:15',1),(161,3,'2025-01-12 07:32:17',0),(162,3,'2025-01-12 07:34:04',1),(163,3,'2025-01-12 07:34:06',0),(164,3,'2025-01-12 07:35:43',1),(165,3,'2025-01-12 07:35:45',0),(166,3,'2025-01-12 07:37:43',1),(167,3,'2025-01-12 07:37:45',0),(168,3,'2025-01-12 07:39:06',1),(169,3,'2025-01-12 07:39:08',0),(170,3,'2025-01-12 07:40:55',1),(171,3,'2025-01-12 07:40:57',0),(172,3,'2025-01-12 07:44:42',1),(173,3,'2025-01-12 07:44:44',0),(174,3,'2025-01-12 07:46:13',1),(175,3,'2025-01-12 07:46:15',0),(176,3,'2025-01-12 07:48:10',1),(177,3,'2025-01-12 07:48:12',0),(178,3,'2025-01-12 07:48:57',1),(179,3,'2025-01-12 07:48:59',0),(180,2,'2025-01-12 08:00:52',1),(181,2,'2025-01-12 08:01:01',0),(182,2,'2025-01-12 08:01:31',1),(183,2,'2025-01-12 08:01:33',0),(184,2,'2025-01-12 08:01:33',0),(185,3,'2025-01-12 08:01:33',1),(186,1,'2025-01-12 08:01:44',1),(187,1,'2025-01-12 08:01:46',0),(188,3,'2025-01-12 08:01:47',0),(189,1,'2025-01-12 08:01:48',1),(190,1,'2025-01-12 08:02:27',0),(191,3,'2025-01-12 08:06:36',1),(192,3,'2025-01-12 08:06:38',0),(193,3,'2025-01-12 08:06:41',1),(194,3,'2025-01-12 08:06:45',0),(195,3,'2025-01-12 08:07:03',1),(196,3,'2025-01-12 08:07:07',0),(197,3,'2025-01-12 08:07:14',1),(198,3,'2025-01-12 08:07:20',0),(199,3,'2025-01-12 08:50:07',1),(200,3,'2025-01-12 08:50:09',0),(201,3,'2025-01-12 08:50:36',1),(202,3,'2025-01-12 08:50:39',0),(203,3,'2025-01-12 08:52:51',1),(204,3,'2025-01-12 08:52:53',0),(205,3,'2025-01-12 08:54:16',1),(206,3,'2025-01-12 08:54:18',0),(207,3,'2025-01-12 08:54:22',1),(208,3,'2025-01-12 08:54:24',0),(209,3,'2025-01-12 09:01:51',1),(210,3,'2025-01-12 09:01:53',0),(211,3,'2025-01-12 09:05:22',1),(212,3,'2025-01-12 09:05:23',0),(213,3,'2025-01-12 09:18:51',1),(214,3,'2025-01-12 09:18:53',0),(215,3,'2025-01-12 09:21:23',1),(216,3,'2025-01-12 09:21:25',0),(217,2,'2025-01-12 09:24:36',1),(218,2,'2025-01-12 09:24:47',0),(219,1,'2025-01-12 09:24:49',1),(220,1,'2025-01-12 09:25:31',0),(221,2,'2025-01-12 09:25:48',1),(222,2,'2025-01-12 09:26:00',0),(223,1,'2025-01-12 09:26:02',1),(224,1,'2025-01-12 09:26:38',0),(225,3,'2025-01-12 09:27:27',1),(226,3,'2025-01-12 09:27:29',0),(227,3,'2025-01-12 09:27:53',1),(228,3,'2025-01-12 09:27:55',0),(229,2,'2025-01-12 09:34:50',1),(230,2,'2025-01-12 09:34:59',0),(231,1,'2025-01-12 09:35:09',1),(232,1,'2025-01-12 09:35:41',0),(233,3,'2025-01-12 09:44:32',1);
/*!40000 ALTER TABLE `historico_dispositivo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-01-12  9:25:07
