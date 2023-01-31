[Confira o tutorial aqui](https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_Dissolved_Oxygen_Sensor_SKU:SEN0237)


## Especificação
- Sonda de Oxigênio Dissolvido
  - Tipo: Sonda Galvânica
  - Faixa de Detecção: 0~20 mg/L
  - Faixa de temperatura: 0~40 ℃
  - Tempo de resposta: até 98% de resposta total, em 90 segundos (25 ℃)
  - Faixa de pressão: 0~50 PSI
  - Vida útil do eletrodo: 1 ano (uso normal)
  - Período de manutenção:
    - Período de substituição da tampa da membrana:
      - 1~2 meses (em água lamacenta);
      - 4~5 meses (em água limpa)
    - Período de substituição da solução de enchimento: uma vez por mês
  - Comprimento do cabo: 2 metros
  - Conector da Sonda: BNC
- Placa conversora de sinal
  - Tensão de alimentação: 3,3~5,5V
  - Sinal de saída: 0~3,0V
  - Conector do cabo: BNC
  - Conector de Sinal: Interface Analógica de Gravidade (PH2.0-3P)
  - Dimensão: 42 mm * 32 mm/1,65 * 1,26 polegadas


## Board Overview
![N|Solid](https://raw.githubusercontent.com/DFRobot/Wiki/master/Resource/Oxygen_board_function.png)

| No.	| Label | Descrição
| ------ | ------ | ------ |
| 1	| A	| Saída de Sinal Analógico (0~3.0V)
| 2	| +	| VCC (3.3~5.5V)
| 3	| -	| GND
| 4	| BNC	| Conector do Cabo da Sonda


## Diagrama de conexão
Quando a sonda é preenchida com solução de NaOH, ela precisa ser calibrada. Antes da calibração, conecte a sonda conforme mostrado no diagrama a seguir. Conecte a ponta de prova ao conector BNC na placa do conversor de sinal. Conecte a placa à entrada analógica da placa principal do Arduino.

<img src="https://user-images.githubusercontent.com/37712508/215746679-cbc7919f-2d47-4e8b-b1d7-e864aa346135.png" alt="drawing" width="600"/>


