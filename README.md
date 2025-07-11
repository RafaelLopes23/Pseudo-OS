# Sistema de Monitoramento Cardíaco em Rust

Este projeto implementa um sistema de monitoramento cardíaco em tempo real seguindo os princípios de sistemas embarcados, desenvolvido em Rust.

## Visão Geral

O sistema monitora sinais de ECG (eletrocardiograma) simulados, detecta anomalias em tempo real e registra eventos. Foi projetado para atender aos requisitos de um sistema de tempo real com tarefas periódicas e aperiódicas.

## Arquitetura do Sistema

### Modelo de Tarefas

| ID | Tarefa                  | Tipo        | Período (T) | Deadline (D) | WCET (Ce) | Precedência |
|----|-------------------------|-------------|-------------|--------------|-----------|-------------|
| T1 | Aquisição do ECG        | Periódica   | 5 ms        | 5 ms         | 1 ms      | –           |
| T2 | Pré-processamento       | Periódica   | 5 ms        | 5 ms         | 1 ms      | T1 → T2     |
| T3 | Detecção de anomalias   | Aperiód./Esp| – (trigger) | 2 ms (rel.)  | 0.5 ms    | T2 → T3     |
| T4 | Log/Comunicação         | Periódica   | 50 ms       | 50 ms        | 5 ms      | T3 → T4     |
| T5 | Interface/HMI           | Periódica   | 100 ms      | 100 ms       | 3 ms      | –           |

### Características das Tarefas

- **T1 (Aquisição ECG)**: Lê dados do arquivo CSV simulando um ADC
- **T2 (Pré-processamento)**: Filtra e normaliza os sinais
- **T3 (Detecção de anomalias)**: Disparada por interrupção de software quando T2 produz amostra
- **T4 (Log/Comunicação)**: Registra eventos e comunica com sistemas externos
- **T5 (Interface/HMI)**: Mantém interface de usuário atualizada

### Detecção de Anomalias

O sistema detecta anomalias baseado nos seguintes critérios:
- **Tensão Alta**: > 10.0 mV
- **Tensão Baixa**: < 7.0 mV

## Estrutura do Projeto

```
src/
├── main.rs                    # Sistema principal e coordenação
├── ecg_data.rs               # Estruturas para dados ECG e leitura CSV
├── tasks.rs                  # Implementação das 5 tarefas do sistema
├── scheduler.rs              # Scheduler de tempo real e análise RMA/RTA
├── wcrt_analysis.rs          # Análise WCRT e Response-Time Analysis
├── performance_collector.rs  # Coleta de métricas em tempo real e relatórios JSON
├── real_time_measurements.rs # Medições de alta precisão
├── metrics.rs               # Métricas de performance básicas
├── interrupts.rs            # Gerador e estatísticas de interrupções
├── anomaly_detector.rs      # Detector de anomalias cardíacas
└── lib.rs                   # Biblioteca principal
```

### Arquivos de Configuração e Dados
```
├── Cargo.toml                     # Dependências e configuração do projeto
├── ecg_input.csv                  # Dados ECG de entrada (6000 amostras)
├── Dockerfile                     # Containerização para execução sem Rust
├── DOCKER_COMMANDS_NEW.txt        # Comandos Docker para execução
└── performance_report_*.json      # Relatórios de performance gerados
```

## Algoritmos de Scheduling

### Rate Monotonic Analysis (RMA) - `src/scheduler.rs`
- Prioridades atribuídas inversamente proporcionais aos períodos
- Verificação de schedulability usando limite de utilização: `U ≤ n(2^(1/n) - 1)`
- Função `analyze_schedulability()` implementa o algoritmo completo

### Response Time Analysis (RTA) - `src/wcrt_analysis.rs`
- Cálculo de WCRT (Worst-Case Response Time) usando iteração até convergência
- Fórmula implementada: `Rq = Cq + Jq + Bq + ∑(i∈hp(q)) ⌈(Rq + Ji)/Ti⌉ * Ci`
- Considera jitter, blocking time e interferência de tarefas de maior prioridade
- Função `calculate_wcrt()` com loop de convergência

### Atualização Dinâmica de WCET - `src/wcrt_analysis.rs`
- WCET inicial: valores estáticos em `create_default_task_characteristics()`
- WCET atualizado: percentil 99 ou 1.2×média das medições reais
- Jitter atualizado: 2×desvio padrão das medições
- Função `update_from_real_measurements()` integra dados reais com teoria

### Prioridades das Tarefas
1. T3 (Detecção de anomalias) - Prioridade 1 (mais alta)
2. T1 (Aquisição ECG) - Prioridade 2
3. T2 (Pré-processamento) - Prioridade 3
4. T4 (Log/Comunicação) - Prioridade 4
5. T5 (Interface/HMI) - Prioridade 5 (mais baixa)

## Requisitos Atendidos

### Requisito 1: Aplicação Real e Factível
**Localização**: `src/main.rs`, `src/tasks.rs`, `src/ecg_data.rs`
- Sistema de monitoramento cardíaco com processamento em tempo real
- Adequado para prazos de implementação e demonstração
- Processa dados ECG reais de arquivo CSV com 6000 amostras

### Requisito 2: Especificação de Tarefas
**Localização**: `src/wcrt_analysis.rs` (função `create_default_task_characteristics()`)
- Deadlines, períodos, WCET claramente definidos na tabela de tarefas
- Relacionamentos de precedência especificados em `src/tasks.rs`
- Prioridades atribuídas usando Rate Monotonic (período menor = prioridade maior)

### Requisito 3: Documentação e Implementação
**Localização**: `src/wcrt_analysis.rs`, `src/performance_collector.rs`
- Tempos de computação estimados e medidos em tempo real
- Simulações funcionais e temporais com coleta de estatísticas
- Relatórios JSON detalhados com métricas completas

### Requisito 4: Tarefas Periódicas e Aperiódicas
**Localização**: `src/tasks.rs`, `src/interrupts.rs`
- T1, T2, T4, T5: Periódicas (implementadas com `tokio::time::interval`)
- T3: Aperiódica/Esporádica disparada por detecção de anomalias
- Gerador de interrupções aleatórias em `src/interrupts.rs`
- Executivo cíclico + interrupções de software simuladas

### Requisito 5: Análise WCRT
**Localização**: `src/wcrt_analysis.rs`, `src/scheduler.rs`
- Implementação completa de RTA (Response-Time Analysis)
- Fórmula: `Rq = Cq + Jq + Bq + ∑(i∈hp(q)) ⌈(Rq + Ji)/Ti⌉ * Ci`
- Verificação de schedulability usando Rate Monotonic bounds
- Comparação entre WCRT teórico e tempos reais medidos

### Requisito 6: Coleta de Métricas em Tempo Real
**Localização**: `src/performance_collector.rs`
- Medição de tempos de execução de todas as tarefas
- Cálculo de percentis (95%, 99%), médias e estatísticas
- Geração de relatórios JSON com dados completos
- Atualização dinâmica de WCET baseado em medições reais

## Como Executar

### Pré-requisitos
- Rust 1.70+ instalado
- Arquivo `ecg_input.csv` no diretório raiz (incluído no projeto)

### Execução Local

```bash
# Compilar o projeto
cargo build --release

# Executar o sistema (30 segundos de execução)
cargo run --release

# Executar com logs detalhados
RUST_LOG=info cargo run --release

# Executar testes
cargo test
```

### Execução com Docker (Recomendado para Apresentações)

```bash
# Construir a imagem Docker
docker build -t cardiac-monitor .

# Executar o container
docker run --rm cardiac-monitor

# Salvar saída completa em arquivo
docker run --rm cardiac-monitor > output.txt 2>&1

# Extrair apenas relatório final (Linux/macOS)
docker run --rm cardiac-monitor 2>&1 | \
  sed -n '/Sistema de monitoramento parado/,$p' > final_report.txt

# Extrair apenas relatório final (Windows PowerShell)
docker run --rm cardiac-monitor 2>&1 | `
  Select-String 'Sistema de monitoramento parado' -Context 0,200 > final_report.txt
```

### Saída Esperada

O sistema produzirá:
1. **Logs de execução**: Aquisição ECG, detecção de anomalias, métricas
2. **Relatório JSON**: `performance_report_*.json` com estatísticas completas
3. **Análise WCRT**: Comparação teórico vs. real no console

### Demonstração ao Vivo

Para demonstrar mudanças de escalonabilidade:
1. Editar `src/wcrt_analysis.rs` → `create_default_task_characteristics()`
2. Modificar WCET: `Duration::from_micros(500)` → `Duration::from_micros(400)`
3. Recompilar e executar: `cargo run --release`
4. Observar mudança de "❌ NÃO" para "✅ SIM" em "Sistema Escalonável"

## Métricas e Análise

### Métricas Coletadas (`src/performance_collector.rs`)
- **Tempos de execução**: Min, max, média, mediana, percentis 95%/99%
- **Deadline misses**: Detecção via `src/scheduler.rs`
- **Utilização de CPU**: Calculada por tarefa e total do sistema
- **WCRT**: Calculado vs. observado com comparação teórico/real
- **Estatísticas de interrupções**: Total, tipos, tempos de serviço
- **Jitter**: Variabilidade calculada como 2×desvio padrão

### Relatório JSON Gerado
O arquivo `performance_report_*.json` contém:
```json
{
  "task_statistics": {
    "EcgAcquisition": {
      "total_executions": 6000,
      "min_execution_time_us": 959,
      "max_execution_time_us": 5550,
      "estimated_wcet_us": 5550,
      // ... mais estatísticas
    }
  },
  "interrupt_stats": { /* estatísticas de interrupções */ },
  "scheduling": { /* análise Rate Monotonic */ },
  "wcrt_analysis": { /* resultados teóricos RTA */ }
}
```

### Análise de Performance (`src/wcrt_analysis.rs`)
- **Schedulability**: Verificação usando RMA bounds
- **WCRT vs. Real**: Comparação com razão Real/Teórico
- **Margem de segurança**: Deadline - WCRT calculado
- **Recomendações**: Automáticas baseadas nas razões observadas

## Exemplo de Uso e Análise Comparativa

### Execução Básica
```bash
# Execução local com logs detalhados
RUST_LOG=info cargo run --release

# Execução via Docker
docker run --rm cardiac-monitor
```

### Análise Comparativa entre Execuções
```bash
# Múltiplas execuções para comparação
for i in {1..3}; do
  echo "=== Execução $i ===" | tee -a comparison.txt
  docker run --rm cardiac-monitor 2>&1 | tee -a comparison.txt
  echo "" | tee -a comparison.txt
done

# Comparar apenas métricas finais
grep -A 20 "Task Performance Summary" comparison.txt
```

### Demonstração de Mudança de Escalonabilidade
Para mostrar o impacto de alterações de WCET:
1. **Editar** `src/wcrt_analysis.rs` → função `create_default_task_characteristics()`
2. **Modificar** WCET da EcgAcquisition: `Duration::from_micros(500)` → `Duration::from_micros(400)`
3. **Recompilar**: `cargo build --release`
4. **Executar**: `cargo run --release`
5. **Observar**: Mudança de "❌ NÃO" para "✅ SIM" em "Sistema Escalonável"

### Saída Esperada
O sistema produzirá:
- **Logs de execução**: Aquisição ECG, detecção de anomalias, métricas em tempo real
- **Relatório JSON**: `performance_report_*.json` com estatísticas completas
- **Análise WCRT**: Comparação teórico vs. real no console final
- **Recomendações**: Baseadas nas razões Real/Teórico observadas

## Tecnologias e Dependências

### Linguagem e Runtime
- **Rust 1.70+**: Linguagem principal para garantir segurança de memória e performance
- **Tokio**: Runtime assíncrono para concorrência e tarefas periódicas

### Bibliotecas Principais (`Cargo.toml`)
- **CSV**: Leitura e parsing de dados ECG de entrada
- **Chrono**: Manipulação precisa de timestamps e duração
- **Crossbeam**: Estruturas de dados thread-safe para comunicação entre tarefas
- **Log/env_logger**: Sistema robusto de logging com níveis configuráveis
- **Serde**: Serialização JSON para relatórios de performance

### Containerização
- **Docker**: Multi-stage build com Rust 1.75-slim
- **Usuário não-root**: Para execução segura em ambientes de produção

## Validação e Testes

O projeto inclui testes unitários para:
- Leitura de dados ECG
- Detecção de anomalias
- Métricas de performance
- Análise de schedulability

Execute os testes com:
```bash
cargo test -- --nocapture
```

## Considerações de Tempo Real

### Previsibilidade
- WCETs estimados e verificados
- Scheduling determinístico
- Análise off-line de schedulability

### Responsividade
- Interrupções de software para tarefas críticas
- Prioridades baseadas em criticidade
- Monitoramento contínuo de deadlines

### Confiabilidade
- Detecção de deadline misses
- Métricas de performance em tempo real
- Tratamento robusto de erros

## Limitações e Trabalhos Futuros

### Limitações Atuais
- Simulação via CSV (não hardware real)
- Threading em userspace (não kernel real-time)
- Scheduling por prioridades fixas apenas

### Melhorias Futuras
- Integração com hardware ADC real
- Kernel de tempo real (RTIC)
- Algoritmos de scheduling mais avançados
- Interface gráfica para HMI
