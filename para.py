import time
import os
import numpy as np
from tensorflow.keras.preprocessing.image import load_img, img_to_array
import tensorflow as tf
import tensorflow_hub as hub
import psutil
import concurrent.futures

# Définir les chemins des dossiers d'images
input_parallele = '/home/yasser_jemli/syscalls/input'
output_parallele = '/home/yasser_jemli/syscalls/output/parallele'

# Charger le modèle depuis TensorFlow Hub
def load_model():
    model_url = "https://tfhub.dev/google/tf2-preview/mobilenet_v2/classification/4"
    model = tf.keras.Sequential([hub.KerasLayer(model_url, input_shape=(224, 224, 3))])
    return model

model = load_model()

# Fonction pour mesurer l'utilisation du CPU et la température
def get_system_metrics():
    cpu_usage = psutil.cpu_percent(interval=1)
    try:
        temperature = psutil.sensors_temperatures()['coretemp'][0].current
    except (KeyError, IndexError):
        temperature = "N/A"  # Température non disponible
    return cpu_usage, temperature

# Fonction de prétraitement d'une seule image
def preprocess_image(image_path):
    image = load_img(image_path, target_size=(224, 224))
    image_array = img_to_array(image)
    image_array = np.expand_dims(image_array, axis=0)
    image_array = tf.keras.applications.mobilenet_v2.preprocess_input(image_array)
    output_path = os.path.join(output_parallele, os.path.basename(image_path))
    np.save(output_path, image_array)
    return output_path

# Fonction de prétraitement parallèle de toutes les images
def preprocess_images_parallel(input_parallele, output_parallele):
    if not os.path.exists(output_parallele):
        os.makedirs(output_parallele)
    
    image_paths = [os.path.join(input_parallele, filename) for filename in os.listdir(input_parallele) if filename.endswith(('.png', '.jpg', '.jpeg'))]
    
    start_time = time.time()  # Début de la mesure du temps
    with concurrent.futures.ProcessPoolExecutor() as executor:
        futures = [executor.submit(preprocess_image, image_path) for image_path in image_paths]
        for future in concurrent.futures.as_completed(futures):
            try:
                result = future.result()
                print(f"Processed {result}")
            except Exception as exc:
                print(f"Generated an exception: {exc}")
    end_time = time.time()  # Fin de la mesure du temps
    
    cpu_usage, temperature = get_system_metrics()
    print(f"Parallel preprocessing time: {end_time - start_time:.2f} seconds")
    print(f"CPU Usage: {cpu_usage}%")
    print(f"Temperature: {temperature}°C")

# Fonction d'inférence sur une image avec le modèle
def process_image_with_model(image_path, model):
    image = np.load(image_path)
    predictions = model.predict(image)
    decoded_predictions = tf.keras.applications.mobilenet_v2.decode_predictions(predictions, top=3)[0]
    return os.path.basename(image_path), decoded_predictions

# Fonction d'inférence parallèle sur toutes les images prétraitées
def process_images_parallel(output_parallele, model):
    image_paths = [os.path.join(output_parallele, filename) for filename in os.listdir(output_parallele) if filename.endswith('.npy')]
    
    start_time = time.time()  # Début de la mesure du temps
    with concurrent.futures.ProcessPoolExecutor() as executor:
        futures = [executor.submit(process_image_with_model, image_path, model) for image_path in image_paths]
        for future in concurrent.futures.as_completed(futures):
            try:
                filename, predictions = future.result()
                print(f"Predictions for {filename}: {predictions}")
            except Exception as exc:
                print(f"Generated an exception: {exc}")
    end_time = time.time()  # Fin de la mesure du temps
    
    cpu_usage, temperature = get_system_metrics()
    print(f"Parallel inference time: {end_time - start_time:.2f} seconds")
    print(f"CPU Usage: {cpu_usage}%")
    print(f"Temperature: {temperature}°C")

# Exécution du prétraitement et de l'inférence parallèles
preprocess_images_parallel(input_parallele, output_parallele)
process_images_parallel(output_parallele, model)
