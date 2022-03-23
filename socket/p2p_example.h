#ifndef P2P_EXAMPLE_H
#define P2P_EXAMPLE_H



void p2p_bind_client_socket(int skt);

// 實驗type B, 送給不同server的port是否能相同
void p2p_server_1();
void p2p_client_1();

// 實驗type B, 實測p2p連線效果
void p2p_server_2();
void p2p_client_2();




#endif