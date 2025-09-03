import os
import re
from tqdm import tqdm

# --- Configurações ---
# Pasta onde os livros baixados estão localizados
INPUT_DIR = "gutenberg_corpus"

# Nome do arquivo final que conterá todo o texto limpo e concatenado
OUTPUT_FILE = "corpus_final_limpo.txt"

# --- Funções ---

def clean_text(text):
    """
    Aplica as regras de limpeza a um bloco de texto.
    
    1. Converte para minúsculas.
    2. Substitui tudo que não for (a-z ou espaço) por um espaço.
       Isso é crucial para não unir palavras como "end-of-line".
    3. Substitui sequências de espaços por um único espaço.
    """
    text = text.lower()
    
    # 2. Mantém apenas letras de 'a' a 'z' e espaços.
    #    Qualquer outro caractere é convertido para um espaço.
    text = re.sub(r'[^a-z ]', ' ', text)
    
    # 3. Substitui um ou mais espaços por um único espaço
    text = re.sub(r'\s+', ' ', text)
    
    # Remove espaços no início ou fim do bloco de texto
    return text.strip()


if __name__ == "__main__":
    if not os.path.isdir(INPUT_DIR):
        print(f"Erro: O diretório de entrada '{INPUT_DIR}' não foi encontrado.")
        print("Certifique-se de executar o script de download primeiro.")
        exit()

    try:
        files_to_process = [f for f in os.listdir(INPUT_DIR) if f.endswith(".txt")]
        if not files_to_process:
            print(f"Nenhum arquivo .txt encontrado no diretório '{INPUT_DIR}'.")
            exit()
    except Exception as e:
        print(f"Não foi possível ler o diretório '{INPUT_DIR}': {e}")
        exit()


    print(f"Iniciando a concatenação e limpeza de {len(files_to_process)} arquivos...")

    # Abre o arquivo de saída em modo de escrita ('w')
    # O 'with' garante que o arquivo seja fechado corretamente no final
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f_out:
        # Usa tqdm para criar uma barra de progresso
        for filename in tqdm(files_to_process, desc="Processando arquivos"):
            filepath = os.path.join(INPUT_DIR, filename)
            
            try:
                # Abre cada livro para leitura
                with open(filepath, "r", encoding="utf-8", errors="ignore") as f_in:
                    # Lê todo o conteúdo do livro
                    content = f_in.read()
                    
                    # Limpa o conteúdo
                    cleaned_content = clean_text(content)
                    
                    # Escreve o conteúdo limpo no arquivo de saída,
                    # seguido de um espaço para separar o conteúdo dos livros.
                    f_out.write(cleaned_content + " ")
            except Exception as e:
                print(f"\nNão foi possível processar o arquivo {filename}: {e}")
                
    final_size_bytes = os.path.getsize(OUTPUT_FILE)
    final_size_mb = final_size_bytes / (1024 * 1024)
    
    print("\n--- Processo Concluído! ---")
    print(f"O arquivo final foi salvo como: '{OUTPUT_FILE}'")
    print(f"Tamanho final do corpus: {final_size_mb:.2f} MB")