## Sobre o Projeto Alice

### Para começar
Por favor consulte o docs/contributing.md para mais informações! Ele contém informação sobre como compilar o projeto tanto no Windows quanto no Linux

### Partindo de onde o Open V2 parou

O objetivo do Projeto Alice (nomeado em homenagem a [Alice Roosevelt Longworth](https://en.wikipedia.org/wiki/Alice_Roosevelt_Longworth)) é essencialmente criar uma nova versão do [Open V2](https://github.com/schombert/Open-V2) -- meu projeto anterior para criar um clone do Victoria 2 -- e então terminar com uma versão funcional, com todos os recursos do jogo. Isto significa que, ao menos inicialmente, haverão divergências no fazer das coisas a como o Victoria 2 as fez, simplesmente para manter o projeto focado e no caminho certo. Assim que uma versão 1.0 estiver completa, nós podemos usá-la para novos experimentos. Já em Julho de 2023, eu já consegui basicamente chegar aonde o Open V2 estava, menos algumas deficiências na UI.

Espero poder encontrar algumas pessoas para formar um time de arte que possa recriar os assets que o Victoria 2 usa para dar a esse projeto a sua própria identidade visual única. Eu sei que é pedir muito, mas em contrapartida, não será necessária modelação 3D. Nós já temos acesso a um conjunto de novas bandeiras que podemos usar, cortesia do projeto SOE.

### Em comparação a outros projetos

Em comparação ao SOE (Symphony of Empires) este projeto é um clone mais direto do Victoria 2, enquanto o SOE é um jogo em si. De todo modo, eu costumo pensar dele como um projeto-irmão. Nós temos pessoas aqui envolvidas com ambos (Eu mesmo faço sugestões ocasionais lá), e se você vir algo que fazemos e que eles poderiam usar, estou certo de que eles apreciariam a contribuição. Do mesmo modo, nós vamos usar pelo menos um pouco do seu trabalho.

Em relação ao projeto OpenVic2, apesar de ter sido "divulgado" como um "clone" do Victoria 2, não é para isso que eles estão trabalhando. Na verdade, eles planejam prover um conjunto completamente novo de assets, incluindo eventos, decisões, etc., que farão do seu projeto um jogo similar ao Victoria 2. Ele também é gerenciado de uma maneira muito diferente. Se você está interessado em ambos os projetos Projeto Alice e OpenVic2, sugiro utilizar o teu tempo com ambos os times ou até mesmo se juntar aos dois projetos.

### Licenciamento

Todo código produzido como parte deste projeto será lançado sob a licença GPL3 (como foi o Open V2). A licença para assets de arte dependerá do time de arte, mas eu preferiria alguma forma como o Creative Commons.

### Ferramentas de programação

O projeto em si usa o CMake (sim, todos odiamos o CMake). Idealmente ele deve ser capaz de ser compilado com qualquer um dos três grandes compiladores: MSVC, Clang/LLVM, or GCC. Eu pessoalmente tendo a usar o MSVC, mas também uso o Clang no Windows. Garantir que as coisas funcionem bem no GCC terá de ser o problema de outrém, e se nenhum contribuidor for um usuário ativo de GCC, então a compatibilidade para o GCC pode desaparecer desta lista.

Pessoalmente eu uso o Visual Studio 2022 (a edição grátis e comunitária) para trabalhar no projeto, mas se estás contribuindo, deves ser capaz de utilizar qualquer ferramenta que prefira, incluindo o VS Code, CLion, EMACS, VI e etc.

### Onde o Projeto Alice diverge do Victoria 2?

Inicialmente, o Projeto Alice irá imitar as mecânicas do Victoria 2 da maneira mais próxima que pudermos, com pequenas exceções. Sim, há muitas coisas que poderíamos melhorar. Eu criei o Open V2 com o mindset de que eu poderia simplesmente melhorar algumas coisas aqui e ali conforme ia, e essa foi uma grande razão para o projeto ter sido posto de lado e ultimamente abandonado. Eu não quero cometer este erro de novo, então desta vez vou tentar o máximo possível e resistir à tenção de fazer melhorias até que tenhamos um jogo funcional e com todos os recursos. Dito isto, há algumas alterações que teremos de fazer não importa o quê. O mapa global é simplesmente muito bom para não colocarmos. E existem algumas melhorias QOL (como hiperlinks que te enviar a o que um evento está falando sobre ou um sumário do porque o evento foi acionado) que são fáceis e boas o suficiente que seria vergonhoso deixá-las de fora. Mas nenhum grande reajuste às mecânicas! Eu prometo!
