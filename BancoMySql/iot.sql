use iot;

CREATE TABLE `dispositivo` (
  `iddisp` int NOT NULL AUTO_INCREMENT,
  `name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `pin` int NOT NULL,
  `imgon` varchar(50) NOT NULL,
  `imgoff` varchar(50) NOT NULL,
  `widthimg` int NOT NULL,
  `heightimg` int NOT NULL,
  `paddingimg` varchar(50) NOT NULL,
  `identifier` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `pintype` varchar(20) NOT NULL,
  `showbutton` int NOT NULL,
  PRIMARY KEY (`iddisp`),
  UNIQUE KEY `identifier` (`identifier`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

CREATE TABLE `sensor` (
  `idsensor` int NOT NULL AUTO_INCREMENT,
  `name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `unit` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `imgon` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `imgoff` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `identifier` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  PRIMARY KEY (`idsensor`),
  UNIQUE KEY `identifier` (`identifier`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

CREATE TABLE `historico_dispositivo` (
  `id` int NOT NULL AUTO_INCREMENT COMMENT 'Primary Key',
  `iddisp` int NOT NULL,
  `dateregister` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` int NOT NULL,
  PRIMARY KEY (`id`),
  KEY `iddisp` (`iddisp`),
  CONSTRAINT `historico_dispositivo_ibfk_1` FOREIGN KEY (`iddisp`) REFERENCES `dispositivo` (`iddisp`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

CREATE TABLE `historico_sensor` (
  `id` int NOT NULL AUTO_INCREMENT,
  `idsensor` int NOT NULL,
  `valor` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `dateregister` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idsensor` (`idsensor`),
  CONSTRAINT `historico_sensor_ibfk_1` FOREIGN KEY (`idsensor`) REFERENCES `sensor` (`idsensor`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;