# Documentação

## Resumo

Projeto `CHIP8` para Arduino `ESP32-2432S028R` criado no VSCODE com platformio

Emulador com TouchScreen, onde você consegue selecionar a rom e jogar

## Instalação

Defina as credenciais no arquivo _credentials.h, lá você configura o MODO AP
    
    remova underline _ no nome do arquivo 

Ao fazer a instalação:
    
    Na guia Platformio:
        
        Na Opção Platform:
            Clique em Upload fileSystem image para carregar os assets, e alguns jogos
        
        Na opção General:
            Clique em Upload para carregar o código binário no ESP32

## Selecionar Jogo

Para selecionar uma nova rom, deve apertar o botão (R)

## Para pausar o jogo 

Deve apertar a letra (P)

## ROMS

Para inserir novas roms, remover, ou listar:
    
    Ligue ESP32, pressione 1 para configurar o WI-FI
    ESP32 entrará no modo AP, com as credenciais que foi definido inicialmente
    Conecte seu dispositivo ( Mobile ou Desktop ) diretamente a rede do ESP32
    Coloque o login e senha ( Modo Wifi )
    No ESP32 vai informar o IP que deve ser conectado
    Coloque o login e senha ( Modo WEB )
    lá será listado todos os jogos disponivel onde é possivel remover
    Em baixo tem um campo para fazer upload de jogos .ch8 somente

### Baseado no projeto

https://github.com/rafaeljacov/inochip8/tree/main
