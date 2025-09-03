import os
import requests
import time
import random
import re
from bs4 import BeautifulSoup


OUTPUT_DIR = "gutenberg_corpus"
TARGET_SIZE_MB = 120
TARGET_SIZE_BYTES = TARGET_SIZE_MB * 1024 * 1024

BOOK_DOWNLOAD_URL = "https://www.gutenberg.org/files/{0}/{0}-0.txt"

SEARCH_BASE_URL = "https://www.gutenberg.org/ebooks/search/?sort_order=downloads&lang=en"

PAGES_TO_SCRAPE = 200
REQUEST_TIMEOUT = 30
REQUEST_DELAY = 2 # Pausa de 2 segundos entre as requisições para não sobrecarregar o site

# --- Funções ---

def  get_english_book_ids_from_scraping():
    """
    NOVA ABORDAGEM: Navega pelas páginas de busca do Project Gutenberg para
    extrair os IDs de livros em inglês diretamente do HTML.
    """
    ids = set() # Usar um conjunto para evitar duplicatas automaticamente

    for page_num in range(1, PAGES_TO_SCRAPE + 1):
        # O site pagina os resultados com um start_index (1, 26, 51, ...)
        start_index = 1 + (page_num - 1) * 25
        search_url = f"{SEARCH_BASE_URL}&start_index={start_index}"
        
        print(f"Analisando página de busca {page_num}/{PAGES_TO_SCRAPE}...", end='\r')

        try:
            response = requests.get(search_url, timeout=REQUEST_TIMEOUT)
            response.raise_for_status()

            # Analisa o HTML da página
            soup = BeautifulSoup(response.text, 'lxml')
            
            # Encontra todos os links que correspondem a um livro (têm a classe 'link')
            book_links = soup.find_all('a', class_='link')

            if not book_links:
                print("\nNão foram encontrados mais livros. Terminando a busca.")
                break

            for link in book_links:
                href = link.get('href')
                # O link é no formato '/ebooks/1342'. Extraímos apenas o número.
                if href and '/ebooks/' in href:
                    book_id_match = re.search(r'\d+', href)
                    if book_id_match:
                        ids.add(int(book_id_match.group(0)))
            
            # Pausa para ser respeitoso com o servidor
            time.sleep(REQUEST_DELAY)

        except requests.exceptions.RequestException as e:
            print(f"\nErro ao acessar a página {page_num}: {e}. Pulando para a próxima.")
            continue
    
    id_list = list(ids)
    print(f"\nBusca finalizada. Encontrados {len(id_list)} IDs de livros únicos.")
    random.shuffle(id_list)
    return id_list


def get_directory_size(dir_path):
    total_size = 0
    try:
        for item in os.listdir(dir_path):
            item_path = os.path.join(dir_path, item)
            if os.path.isfile(item_path):
                total_size += os.path.getsize(item_path)
    except FileNotFoundError:
        return 0
    return total_size

def download_book(book_id, dir_name):
    url = BOOK_DOWNLOAD_URL.format(book_id)
    filename = os.path.join(dir_name, f"book_{book_id}.txt")
    print(f"Tentando baixar livro ID {book_id} (Inglês)...")
    try:
        response = requests.get(url, timeout=REQUEST_TIMEOUT)
        if response.status_code == 200:
            response.encoding = 'utf-8'
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(response.text)
            print(f"SUCESSO: Livro ID {book_id} salvo.")
            return True
        else:
            print(f"FALHA: Arquivo de texto para o livro ID {book_id} não encontrado (Status: {response.status_code}).")
            return False
    except requests.exceptions.RequestException:
        print(f"FALHA: Erro de rede para o livro ID {book_id}.")
        return False


if __name__ == "__main__":
    if not os.path.exists(OUTPUT_DIR):
        print(f"Criando o diretório '{OUTPUT_DIR}'...")
        os.makedirs(OUTPUT_DIR)
    else:
        print(f"Diretório '{OUTPUT_DIR}' já existe.")
    
    english_book_ids = get_english_book_ids_from_scraping()

    if not english_book_ids:
        print("Não foi possível obter a lista de livros. Abortando o script.")
        exit()
        
    print(f"\n--- Iniciando download de livros até a pasta atingir {TARGET_SIZE_MB} MB ---\n")
    
    for book_id in english_book_ids:
        if get_directory_size(OUTPUT_DIR) >= TARGET_SIZE_BYTES:
            print("Tamanho alvo do diretório atingido. Parando os downloads.")
            break
        if download_book(book_id, OUTPUT_DIR):
            current_size_mb = get_directory_size(OUTPUT_DIR) / (1024 * 1024)
            print(f"Progresso: {current_size_mb:.2f} MB / {TARGET_SIZE_MB} MB\n")
        time.sleep(1) # Pausa de 1s entre downloads

    final_size_mb = get_directory_size(OUTPUT_DIR) / (1024 * 1024)
    print(f"\n--- Concluído! ---")
    print(f"O tamanho da pasta '{OUTPUT_DIR}' atingiu {final_size_mb:.2f} MB.")