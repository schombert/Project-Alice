# Progresso até julho de 2023

Bem-vindo à atualização ligeiramente atrasada. Eu tenho me concentrado em preparar a [demonstração pública](https://github.com/schombert/Project-Alice/releases/download/v0.0.1-demo/2023-7-8-DEMO.zip), excluindo todas as outras atividades. Mas, antes tarde do que nunca.

## As Grandes Novidades

A grande novidade deste mês é que o projeto finalmente está pronto para ser lançado ao público, embora em um estado muito rudimentar. A demonstração não inclui nenhuma IA, portanto, é o momento perfeito para jogar como o Texas ou algum outro começo difícil. Ou, pelo menos, ir para uma conquista mundial. Atualmente, a "IA" concordará com qualquer coisa que você pedir, independentemente de fazer sentido ou não.

## Progresso da Interface do Usuário

Como não temos modelos 3D, tivemos que introduzir alguns novos ícones 2D para exibir a atividade militar no mapa. Primeiro, temos uma visualização compacta quando você está relativamente afastado.

![compacto](./images/compact.png)

Para os portos, nossa solução é a seguinte:

![porto](./images/port.png)

Quando você dá zoom, pode ver mais informações sobre as unidades individuais:

![unidade](./images/unit.png)

Também temos uma interface de usuário básica para exibir um cerco ou batalha em curso.

![cerco](./images/siege.png)
![batalha](./images/battle.png)

A interface do usuário não está de forma alguma concluída. No entanto, a maior parte de sua funcionalidade essencial está presente e, por enquanto, meu foco será afastar-se da interface do usuário e voltar para os internos do jogo. Você não deve esperar ver grandes mudanças na interface do usuário entre agora e o próximo mês. Na maior parte, farei pequenos ajustes à medida que avançar.

O trabalho no globo também continua. Este mês, Erik adicionou os rios (bem como a lógica interna para detectar travessias de rios). Isso completa a funcionalidade básica do mapa em si, no sentido de que exibe todas as informações necessárias para jogar o jogo. Certamente, há espaço para melhorias estéticas, mas como não temos uma equipe de arte, eu não confiaria muito em isso acontecer.

![rios](./images/rivers.png)

## IA

Agora que o lado do jogador do jogo está em um estado minimamente funcional, chegou a hora de trabalhar na IA. O meu plano actual para a IA é abordá-la basicamente em duas etapas. Primeiro, vou tentar colocar a IA em um estado de funcionamento aproximado, o que significa que ela terá a capacidade de executar todas as ações que um jogador pode, sem se preocupar muito com o quão bem ela joga o jogo. Em seguida, farei uma segunda passagem para tentar elevá-la a um nível minimamente aceitável. *Não* pretendo tentar fazer a IA "interpretar papéis". A IA geralmente seguirá o que vejo como uma estratégia geralmente aplicável de tentar impulsionar a industrialização enquanto se concentra em maximizar os pontos de pesquisa. A IA geralmente jogará de maneira muito egoísta. Provavelmente, só se unirá às suas guerras ofensivas se acreditar que pode obter algo delas; geralmente não fará favores a você apenas porque você tem uma aliança ou altas relações.

Idealmente, eu gostaria que a IA jogasse o mais competentemente possível dentro das regras do jogo. Em particular, se houver algum microgerenciamento tedioso que daria à IA uma vantagem, como "ciclar unidades", acredito que a IA deve explorá-lo se for possível para o jogador fazer o mesmo. Isso não é apenas porque quero que a IA seja um desafio. Também quero motivar melhorias a longo prazo nas próprias regras, tornando os jogadores sujeitos aos mesmos abusos que podem ser aplicados à IA.

## O Fim

Até o próximo mês! (ou, se você não pode esperar tanto, junte-se a nós no [Discord](https://discord.gg/QUJExr4mRn))