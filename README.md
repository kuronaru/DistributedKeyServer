# DistributedKeyServer
2022东南大学本科毕业设计——基于密钥分割的智能终端密钥管理系统设计与实现

## 摘要
随着手机的普及，移动终端承载的信息量超乎想象，近年来不断增多的手机隐私泄露的案件让人们越来越重视手机的数据安全。传统的密码体制虽然在算法安全方面已经相对成熟，但存储安全依旧存在隐患。为了解决密钥本地存储的安全问题，密钥分割方案被提出。方案将密钥分成多个碎片存储在不同的终端上，分散了本地存储的风险。

为了解决密钥分割的问题，论文研究和学习了椭圆曲线数字签名算法和国密 SM2、SM4算法。Shamir秘密共享算法和多方协同数字签名都是实现密钥分割的方法，在此基础上，结合多种方法提出一种实现基于密钥分割的智能终端密钥管理系统所需的协同签名方案，给出了签名算法和分割方法，并制定了详细的方案。 

根据协同签名方案，使用C++编程，利用 Qt 跨平台特性设计跨平台的多线程服务端原型系统，实现多个客户端接入时，多线程地处理协同签名请求，利用数据库管理和定期更新所有的密钥，并编写了大数运算库以支持高位数的密码计算。 

最后进行系统测试，与客户端和数据库连接，验证了原型系统的可行性和计算的正确性，表现出较高的应用价值。 

**关键词**：密钥分割，椭圆曲线数字签名，协同签名，密钥管理系统

## Abstract
With the popularization of mobile phone, the amount of information carried by mobile terminals is beyond imagination. And the increasing number of mobile phone privacy leakage cases in recent years has made people pay more and more attention to the data security. Although the traditional cryptography has been relatively mature in terms of algorithm, there are still hidden dangers in storage security. In order to solve the security problem of local storage of keys, a secret splitting scheme is proposed, which divides the secret into several fragments and stores them on different disks, which decreases the risk of local storage. 

Starting from the basic theory of cryptography, this paper studies the elliptic curve digital signature algorithm, the SM2 and SM4 algorithms, and the Shamir secret sharing algorithm. On this basis, this paper proposes a collaborative signature scheme and secret splitting scheme, describes the signature algorithm and segmentation method used in the scheme and formulates a detailed scheme process. 

Based on the collaborative signature and secret splitting scheme, this paper uses C++ to design and implement a multi-threaded co-signing and key management server system based on TCP socket, and writes a large number operation library to support high-bit cryptographic computation.  

Finally, the system test is carried out to verify the feasibility of the system and the correctness of the calculation, and has good computational efficiency. 

**KEY WORDS**: Secret splitting, Elliptic curve digital signature algorithm, Collaborative signature, Key management system
