# Trabalho Prático 1 — Topologias com NS-3

Implementação e análise de topologias de rede utilizando o simulador **NS-3 3.36.1**. O trabalho aborda enlaces point-to-point, uma rede Ethernet/CSMA e duas redes Wi-Fi, com foco em comunicação cliente-servidor, roteamento, atraso fim a fim e resolução ARP.

## Autoria

- **Aluna:** Maria Eduarda Sampaio
- **Disciplina:** Redes de Computadores — UFMG
- **Professor:** Aldri Luiz dos Santos

## Ambiente utilizado

O NS-3 foi instalado no seguinte diretório:

```text
~/ns-allinone-3.36.1/ns-3.36.1
```

## Organização do projeto

```text
Lab1_SAMPAIO_MARIA/
├── Part1/
│   ├── lab1-part1.cc
│   ├── lab1-part1-output.txt
│   └── lab1-part1-screenshot.png
├── Part2/
│   ├── lab1-part2.cc
│   ├── lab1-part2-output.txt
│   ├── lab1-part2-questions.txt
│   ├── lab1-part2-screenshot.png
│   ├── delay-original-topology.png
│   ├── delay-modified-topology.png
│   └── pcaps/
└── Part3/
    ├── lab1-part3.cc
    ├── lab1-part3-output.txt
    ├── lab1-part3-output.png
    ├── lab1-part3-question.txt
    └── delay-wifi-topology.png
```

Os programas foram desenvolvidos a partir dos exemplos `first.cc`, `second.cc` e `third.cc` fornecidos pelo NS-3, conforme solicitado no enunciado do trabalho.

## Preparação

Coloque os arquivos-fonte dentro da pasta `scratch` da instalação do NS-3, preservando os subdiretórios usados nos comandos. Em seguida, acesse o diretório principal do simulador:

```bash
cd ~/ns-allinone-3.36.1/ns-3.36.1
```

A versão instalada pode ser conferida com:

```bash
cat VERSION
```

## Parte 1 — Enlaces point-to-point

A primeira parte modifica o exemplo `first.cc` para criar um servidor central conectado diretamente a até cinco clientes. Cada enlace possui taxa de 5 Mbps, atraso de propagação de 2 ms e uma sub-rede IPv4 própria.

Parâmetros:

- `nClients`: número de clientes, entre 1 e 5;
- `nPackets`: número de pacotes enviados por cliente, entre 1 e 5.

O servidor UDP Echo utiliza a porta 15. Os clientes enviam pacotes de 1024 bytes em intervalos de um segundo e iniciam em instantes aleatórios entre 2 e 7 segundos.

Execução utilizada na entrega:

```bash
./ns3 run "scratch/Lab1-part1 --nClients=5 --nPackets=4" 2>&1 | tee scratch/Lab1-part1-output.txt
```

O comando executa a simulação com cinco clientes e quatro pacotes por cliente.
O redirecionamento `2>&1` reúne as saídas padrão e de erro, enquanto `tee` mostra o resultado no terminal e o salva em um arquivo de texto.

Foram produzidas 20 requisições e 20 respostas, sem perda observada. O tempo entre o envio de uma requisição e o recebimento da resposta foi de aproximadamente 7,37 ms.

## Parte 2 — Rede Ethernet/CSMA

A segunda parte modifica o exemplo `second.cc`. Um segundo enlace point-to-point conecta o último nó da rede CSMA a um novo servidor UDP Echo.

Configurações principais:

- enlaces point-to-point: 5 Mbps e 2 ms;
- rede CSMA: 100 Mbps e 6560 ns;
- pacotes: 1024 bytes;
- servidor UDP Echo: porta 9;
- `nPackets`: entre 1 e 20.

Execução utilizada na entrega:

```bash
./ns3 run "scratch/part2/lab1-part2 --nCsma=4 --nPackets=10"
```

A execução utiliza quatro nós CSMA adicionais, totalizando cinco nós na rede local, e envia dez pacotes. As capturas PCAP permitem analisar o percurso dos pacotes, os atrasos e as mensagens ARP.

### Resultados de atraso

| Topologia | Primeiro RTT | RTT dos pacotes 2 a 10 |
|---|---:|---:|
| Original | 24,607 ms | aproximadamente 7,558 ms |
| Modificada | 31,980 ms | aproximadamente 14,929 ms |

Os valores regulares são superiores às estimativas baseadas somente no atraso de propagação porque também existe o tempo de transmissão. Para um quadro de 1054 bytes em um enlace de 5 Mbps:

```text
t_tx = (1054 × 8) / (5 × 10⁶) ≈ 1,686 ms
```

As estimativas são aproximadamente 7,373 ms para a topologia original e 14,746 ms para a modificada.

O primeiro RTT é maior devido às resoluções ARP iniciais. Antes do encaminhamento, os nós precisam descobrir os endereços MAC dos próximos saltos. Após as trocas ARP Request e ARP Reply, os mapeamentos permanecem na cache e os pacotes seguintes apresentam atrasos menores e praticamente constantes.

## Parte 3 — Redes Wi-Fi

A terceira parte modifica o exemplo `third.cc` e substitui a rede Ethernet por uma segunda rede Wi-Fi. Cada rede possui um ponto de acesso e o mesmo número de estações. Os pontos de acesso são conectados por um enlace point-to-point de 5 Mbps e 2 ms.

As redes usam canais físicos e SSIDs distintos. As estações utilizam o modelo `RandomWalk2dMobilityModel`, enquanto os pontos de acesso permanecem fixos.

Parâmetros:

- `nWifi`: número de estações em cada rede, entre 1 e 9;
- `nPackets`: número de pacotes, entre 1 e 20.

Execução utilizada na entrega:

```bash
./ns3 run "scratch/part3/lab1-part3 --nWifi=4 --nPackets=10" 2>&1 | tee lab1-part3-output.txt
```

Foram enviadas dez requisições e recebidas dez respostas, sem perdas observadas. O primeiro RTT foi de aproximadamente 30,36 ms. Para os pacotes de 2 a 10, os atrasos variaram entre aproximadamente 7,77 ms e 7,96 ms.

Ao contrário dos enlaces point-to-point e CSMA, que possuem taxas configuradas diretamente, o Wi-Fi seleciona a taxa de transmissão de forma dinâmica. A contenção pelo canal, o backoff, as confirmações da camada MAC, possíveis retransmissões e a mobilidade das estações provocam pequenas variações no atraso.

## Principais conclusões

Os experimentos mostram que o atraso de uma comunicação não depende apenas do valor de propagação configurado. Também influenciam o resultado:

- a taxa de transmissão;
- o tamanho dos pacotes;
- a quantidade de enlaces percorridos;
- o tempo de transmissão em cada enlace;
- a resolução e a cache ARP;
- o método de acesso ao meio;
- a adaptação de taxa e a mobilidade em redes Wi-Fi.

O trabalho permitiu relacionar os conceitos de endereçamento IP, roteamento, aplicações cliente-servidor e protocolos de enlace com o comportamento observado nas simulações.

## Relatório

O relatório completo apresenta a descrição do ambiente, as configurações das topologias, capturas das execuções, gráficos de atraso e a análise dos resultados.
