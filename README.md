# Pseudo Sistema Operacional

Este projeto implementa um pseudo-sistema operacional em C puro, projetado para fins educacionais. O sistema é estruturado para permitir desenvolvimento colaborativo e inclui módulos para gerenciamento de processos, gerenciamento de memória, gerenciamento de recursos de E/S e operações do sistema de arquivos.

## Estrutura do Projeto

O projeto está organizado em vários diretórios:

- **src/**: Contém o código-fonte do sistema operacional.
  - **core/**: Funcionalidades principais incluindo o ponto de entrada principal e definições do sistema.
  - **process/**: Gerencia processos, incluindo escalonamento e despacho.
  - **memory/**: Trata da alocação de memória e gerenciamento de recursos de E/S.
  - **filesystem/**: Gerencia operações do sistema de arquivos e manipulação de arquivos.

- **include/**: Contém arquivos de cabeçalho compartilhados com constantes e definições de erro.

- **tests/**: Contém testes unitários para cada módulo para garantir funcionalidade e confiabilidade.

- **examples/**: Fornece arquivos de entrada de exemplo para testar o sistema.

- **Makefile**: Contém instruções de compilação para construir o projeto.

- **.gitignore**: Especifica arquivos e diretórios a serem ignorados pelo Git.

## Compilando o Projeto

Para compilar o projeto, navegue até o diretório raiz do projeto e execute:

```
make
```

Isso compilará todos os módulos e criará o executável para o pseudo-sistema operacional.

## Executando o Sistema

Após compilar o projeto, você pode executar o sistema usando o seguinte comando:

```
./pseudo-os
```

Certifique-se de fornecer os arquivos de entrada necessários (`processes.txt` e `files.txt`) no formato apropriado conforme especificado na documentação do projeto.

## Contribuindo

Este projeto está aberto para contribuições. Por favor, siga as diretrizes abaixo:

1. Faça um fork do repositório.
2. Crie uma nova branch para sua funcionalidade ou correção de bug.
3. Faça suas alterações e commit-as.
4. Push sua branch e crie um pull request.

## Licença

Este projeto está licenciado sob a Licença MIT. Veja o arquivo LICENSE para mais detalhes.