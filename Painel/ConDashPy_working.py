
import datetime
import time
import pygame
import serial

# inicialização
pygame.init()
pygame.joystick.init()
controle = pygame.joystick.Joystick(0)
controle.init()
arduino = serial.Serial("COM6", 115200)
time.sleep(2)


def formatacao(valor):
    byteint = str(round(((valor + 1) * 127)))
    return byteint


def enviararduino(mandarstr):
    arduino.write(mandarstr.encode('ascii'))


# função que recebe e processa os dados que o arduino devolve
def receberarduino():
    global inicioChar, fimChar

    ck = ""
    x = "z"  # qualquer valor que não seja iniciochar/fimchar
    bytecount = -1  # to allow for the fact that the last increment will be one too many

    # espera o inicioChar para começar a leitura
    while ord(x) != inicioChar:
        x = arduino.read()

    # salva dados até o fimChar chegar
    while ord(x) != fimChar:
        if ord(x) != inicioChar:
            ck = ck + x.decode("utf-8")
            bytecount += 1
        x = arduino.read()

    return ck


def esperararduino():
    # espera que o arduino mande "arduino esta pronto" -- dá tempo para que ele reinicie após a conexão serial
    # também garante que qualquer byte previamente armazenado seja apagado

    global inicioChar, fimChar

    msg = ""
    while msg.find("Arduino esta pronto") == -1:

        while arduino.inWaiting() == 0:
            pass

        msg = receberarduino()

        print(msg)  # python3 requires parenthesis
        print()


def testar(td):
    numloops = len(td)
    aguardandoresposta = False

    n = 0
    while n < numloops:
        teststr = td[n]

        if aguardandoresposta is False:
            enviararduino(teststr)
            print("Enviado pelo PC, teste #" + str(n) + ", valor enviado: " + teststr)
            aguardandoresposta = True

        if aguardandoresposta is True:

            while arduino.inWaiting() == 0:
                pass

            dadosrecebidos = receberarduino()
            print("Resposta recebida: " + dadosrecebidos)
            n += 1
            aguardandoresposta = False
        print("")

def enviarinfo(info):
    numloops = len(info)

    n = 0
    if n < numloops:
        informacaoaenviar = info[n]
        enviararduino(informacaoaenviar)
        print("Enviado pelo PC, teste #" + str(n) + ", valor enviado: " + informacaoaenviar)
        print("")


# variaveis e configs
rodar = True
white = (255, 255, 255)
fundo = pygame.image.load("fundo.png")
fundo = pygame.transform.scale(fundo, (1200, 440))
fonte = pygame.font.Font("secretcode.ttf", 32)
fonte2 = pygame.font.Font("calibri.ttf", 18)
tela = pygame.display.set_mode((1200, 440))
pygame.display.set_caption("ControlDashPy")
inicioChar = 60
fimChar = 62
t = 0
rafinha = 0

print("Porta Serial COM5" + " aberta com Baudrate " + str(115200))
esperararduino()

while rodar:
    pygame.event.pump()  # atualiza os valores do joystick
    data = datetime.datetime.now()
    mes = data.strftime("%m")
    hora = data.strftime("%H")  # atualiza as variáveis
    dia = data.strftime("%d")
    minuto = data.strftime("%M")
    diames = fonte2.render(dia + "/" + mes, True, white)
    horamin = fonte2.render(hora + ":" + minuto, True, white)
    left_x = controle.get_axis(0)
    left_y = controle.get_axis(1)
    right_x = controle.get_axis(4)  # define o nome dos eixos do controle
    right_y = controle.get_axis(3)
    gatilho = controle.get_axis(2)
    botaoA = controle.get_button(0)
    botaoB = controle.get_button(1)
    botaoX = controle.get_button(2)
    botaoY = controle.get_button(3)
    botaoLb = controle.get_button(4)
    botaoRb = controle.get_button(5)
    botaoBack = controle.get_button(6)
    botaoStart = controle.get_button(7)
    xhat, yhat = controle.get_hat(0)
    if xhat == 1:
        dX = 1
        ndX = 0
    elif xhat == -1:
        ndX = 1
        dX = 0
    else:
        dX = 0
        ndX = 0
    if yhat == 1:
        dY = 1
        ndY = 0
    elif yhat == -1:
        ndY = 1
        dY = 0
    else:
        dY = 0
        ndY = 0

    dados = ["<R, " + formatacao(left_x) + ", " + formatacao(gatilho) + ">"]

    if t >= 5:
        enviarinfo(dados)
        t = 0
        rafinha += 1
        print(rafinha)
    if t < 5:
        t += 1
    if gatilho > 0:
        gatilhocor = (102, 255, 51)
    else:
        gatilhocor = (255, 96, 31)

    valorpainelacc = gatilho * 350
    valorpainelservo = left_x * 190
    if valorpainelacc > 0:
        valorpainelacc = valorpainelacc * -1
    tela.fill((40, 40, 40))
    pygame.draw.rect(tela, gatilhocor, (400, 400, 400, valorpainelacc))
    pygame.draw.rect(tela, (255, 96, 31), (585, 410, valorpainelservo, 20))
    tela.blit(fundo, (0, 0))
    tela.blit(diames, (140, 40))
    tela.blit(horamin, (290, 40))

    pygame.display.update()
