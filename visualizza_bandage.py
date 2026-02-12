import os
import subprocess
import csv
import re

EXECUTABLE = "msp.exe"      
DATA_FOLDER = "DATA"         # Cartella con i GFA
OUTPUT_FOLDER = "CSV_bandage"
COLOR_PATH = "green"         # Colore percorso
COLOR_PEBBLE = "red"         # Colore Pebble

"""funzione che pulisce una stringa da caratteri non contentiti per renderla compatibile con windows"""
def clear_filename(name):
    return re.sub(r'[<>:"/\\|?*]', '_', name)
    
"""Funzione che esegue il parsing di un file GFA per estrarre la struttura dei percorsi"""
def get_gfa_paths(filepath):
    paths_list = []
    try:
        with open(filepath, 'r') as f:
            for line in f:
                parts = line.strip().split('\t')
                if len(parts) > 2 and parts[0] == 'P':
                    raw_name = parts[1]
                    safe_name = clear_filename(raw_name)                 
                    segment_str = parts[2]
                    p_nodes = []
                    for seg in segment_str.split(','):
                        clean_id = seg.replace('+', '').replace('-', '')
                        if clean_id.isdigit():
                            p_nodes.append(int(clean_id))
                    
                    paths_list.append({'name': safe_name, 'nodes': p_nodes})
    except Exception as e:
        print(f"Errore GFA: {e}")
    return paths_list

"""Analizza l'output del codice C per estrarre i pebbles"""
def parse_c_output_for_pebbles(c_output_text):
    pebbles_map = {} 
    current_path_idx = -1
    in_msp2_section = False
    re_path = re.compile(r"Path #(\d+):")
    lines = c_output_text.splitlines()
    for line in lines:
        line = line.strip()
        m_path = re_path.search(line)
        if m_path:
            current_path_idx = int(m_path.group(1))
            in_msp2_section = False 
            if current_path_idx not in pebbles_map:
                pebbles_map[current_path_idx] = set()
            continue
        if "[MSP 2]" in line and "archi" in line:
            in_msp2_section = True
            continue
        if in_msp2_section and "->" in line:
            parts = line.split("->")
            if len(parts) == 2:
                try:
                    u = int(parts[0].strip())  
                    if current_path_idx != -1:
                        pebbles_map[current_path_idx].add(u)
                except ValueError:
                    pass

    return pebbles_map

"""    
    Esegue i seguenti passaggi per ogni file .gfa nella cartella DATA:
    1. Legge la struttura dei percorsi usando Python (per avere tutti i nodi verdi).
    2. Esegue il programma C (msp.exe) in un sottoprocesso e ne cattura l'output.
    3. Analizza l'output del C per scoprire quali nodi sono Pebbles (rossi).
    4. Genera file CSV formattati per Bandage ("name,color") unendo le due informazioni.
"""
def main():
    if not os.path.exists(DATA_FOLDER):
        print("Cartella DATA non trovata.")
        return

    files = [f for f in os.listdir(DATA_FOLDER) if f.endswith(".gfa")]
    print(f"Trovati {len(files)} file GFA.\n")

    for filename in files:
        filepath = os.path.join(DATA_FOLDER, filename)
        base_name = os.path.splitext(filename)[0]
        paths_structure = get_gfa_paths(filepath)
        if not paths_structure: continue
        try:
            res = subprocess.run([EXECUTABLE, filepath], capture_output=True, text=True, encoding="utf-8", errors="ignore")
            pebbles_map = parse_c_output_for_pebbles(res.stdout)
            generated_count = 0
            for i, p_info in enumerate(paths_structure):
                p_nodes = p_info['nodes']
                p_name = p_info['name'] 
                current_pebbles = pebbles_map.get(i, set())
                csv_name = f"{base_name}_{p_name}_bandage.csv"
                csv_path = os.path.join(OUTPUT_FOLDER, csv_name)
                
                with open(csv_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(["name", "color"])
                    
                    for node in p_nodes:
                        if node in current_pebbles:
                            writer.writerow([node, COLOR_PEBBLE])
                        else:
                            writer.writerow([node, COLOR_PATH])
                
                generated_count += 1
            
            print(f"File: {filename} -> Generati {generated_count} CSV.")

        except Exception as e:
            print(f"Errore su {filename}: {e}")

    print("\nFinito.")

if __name__ == "__main__":
    main()