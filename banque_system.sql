SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

CREATE DATABASE IF NOT EXISTS `banque_system` 
DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE `banque_system`;

DROP TABLE IF EXISTS `client`;
DROP TABLE IF EXISTS `accounts`;
DROP TABLE IF EXISTS `operations`;
-- --------------------------------------------------------

CREATE TABLE `client` (
  `CIN` varchar(20) NOT NULL,
  `nom` text NOT NULL,
  `prenom` text NOT NULL,
  `n_tele` text NOT NULL,
  `date_naissance` text NOT NULL,
  `adresse` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `client` (`CIN`, `nom`, `prenom`, `n_tele`, `date_naissance`, `adresse`) VALUES
('BE938445', 'Youssefi', 'Ilyas', '0665186150', '06-07-2004', 'Casablanca, oasis, Rue 10'),
('BK123456', 'Alami', 'Ahmed', '0661123456', '15-05-1990', '12 Rue boulevard Zerktouni, Casablanca'),
('DA654321', 'Mansouri', 'Sara', '0707112233', '12-07-2001', 'Q.I. Sidi Ghanem, Tanger'),
('EE987654', 'Bennani', 'Fatima', '0770987654', '20-11-1995', 'Av. Mohammed V, Apt 4, Rabat'),
('GK998877', 'Haddad', 'Nabil', '0650123456', '03-11-1980', 'Bd Mohamed VI, Casablanca'),
('JM456789', 'Tazi', 'Karim', '0655443322', '30-01-1988', 'Bd Hassan II, Marrakech'),
('JT882211', 'Idrissi', 'Youssef', '0661889900', '14-02-1992', 'Av. Hassan II, Agadir'),
('LA123987', 'Chraibi', 'Omar', '0612345678', '05-09-1985', 'Hay Riad, Secteur 10, Rabat');

-- --------------------------------------------------------

CREATE TABLE `accounts` (
  `num_account` varchar(17) NOT NULL,
  `titulaire` varchar(20) NOT NULL,
  `solde` float DEFAULT NULL,
  `mdp` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `accounts` (`num_account`, `titulaire`, `solde`, `mdp`) VALUES
('1171462791634253', 'BE938445', 100, 'H28PU8H2'),
('4521890012345678', 'BK123456', 5000, 'P1acd8WD'),
('5566778899001122', 'DA654321', 14500, 'aNo8gav9'),
('9876543210987654', 'EE987654', 1200.5, 'KaL9mN3b'),
('9988776655443322', 'GK998877', 50000, 'ao12BnMZ'),
('1111222233334444', 'JM456789', 750, 'MtkL4pQa'),
('4000500060007000', 'JT882211', 8400, 'XyZ3rT9q'),
('6677889900112233', 'LA123987', 500, 'QwErTy12');

-- --------------------------------------------------------

CREATE TABLE `operations` (
  `id_trans` int(11) NOT NULL,
  `type_op` text NOT NULL,
  `account_from` varchar(17) DEFAULT NULL,
  `account_to` varchar(17) DEFAULT NULL,
  `date` text NOT NULL,
  `amount` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `operations` (`id_trans`, `type_op`, `account_from`, `account_to`, `date`, `amount`) VALUES
(1, 'Depot', '4521890012345678', NULL, '2025-01-02 10:15:00', 6000),
(2, 'Depot', '9876543210987654', NULL, '2025-01-03 09:20:00', 2500),
(3, 'Depot', '5566778899001122', NULL, '2025-01-04 14:30:00', 18000),
(4, 'Depot', '9988776655443322', NULL, '2025-01-05 11:45:00', 55000),
(5, 'Retrait', '4521890012345678', NULL, '2025-01-06 16:10:00', 1000),
(6, 'Retrait', '9876543210987654', NULL, '2025-01-07 08:50:00', 800),
(7, 'Retrait', '5566778899001122', NULL, '2025-01-08 13:25:00', 3000),
(8, 'Retrait', '9988776655443322', NULL, '2025-01-09 10:40:00', 5000),
(9, 'Transaction', '9876543210987654', '1111222233334444', '2025-01-10 15:30:00', 750),
(10, 'Transaction', '9876543210987654', '4521890012345678', '2025-01-11 09:15:00', 250),
(11, 'Transaction', '5566778899001122', '6677889900112233', '2025-01-12 12:00:00', 500),
(12, 'Transaction', '4000500060007000', '1171462791634253', '2025-01-13 17:20:00', 100);


-- --------------------------------------------------------


ALTER TABLE `client`
  ADD PRIMARY KEY (`CIN`);

ALTER TABLE `accounts`
  ADD PRIMARY KEY (`num_account`),
  ADD KEY `fk_tit` (`titulaire`);

ALTER TABLE `operations`
  ADD PRIMARY KEY (`id_trans`),
  ADD KEY `fk_oper` (`account_from`);

ALTER TABLE `operations`
  MODIFY `id_trans` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=13;

ALTER TABLE `accounts`
  ADD CONSTRAINT `fk_tit` FOREIGN KEY (`titulaire`) REFERENCES `client` (`CIN`);

ALTER TABLE `operations`
  ADD CONSTRAINT `fk_oper` FOREIGN KEY (`account_from`) REFERENCES `accounts` (`num_account`);

COMMIT;
