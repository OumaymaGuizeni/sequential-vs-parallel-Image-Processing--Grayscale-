import time
import os
import numpy as np
from tensorflow.keras.preprocessing.image import load_img, img_to_array
import tensorflow as tf
import tensorflow_hub as hub
import psutil

# Définir les chemins des dossiers d'images
input_sequentiel = '/home/yasser_jemli/syscalls/input'
output_sequentiel = '/home/yasser_jemli/syscalls/output/sequentiel'

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
    output_path = os.path.join(output_sequentiel, os.path.basename(image_path))
    np.save(output_path, image_array)
    return output_path

# Fonction de prétraitement séquentiel de toutes les images
def preprocess_images_sequential(input_sequentiel, output_sequentiel):
    if not os.path.exists(output_sequentiel):
        os.makedirs(output_sequentiel)
    
    image_paths = [os.path.join(input_sequentiel, filename) for filename in os.listdir(input_sequentiel) if filename.endswith(('.png', '.jpg', '.jpeg'))]
    
    start_time = time.time()  # Début de la mesure du temps
    for image_path in image_paths:
        preprocess_image(image_path)
    end_time = time.time()  # Fin de la mesure du temps
    
    return end_time - start_time

# Fonction d'inférence sur une image avec le modèle
def process_image_with_model(image_path, model):
    image = np.load(image_path)
    
    # Assurez-vous que `image` est un tableau avec la forme attendue (224, 224, 3)
    image = np.squeeze(image, axis=0)  # Supprimez la dimension supplémentaire
    
    # Assurez-vous que `image` a les bonnes dimensions et prétraitements
    image = tf.keras.applications.mobilenet_v2.preprocess_input(image)
    image = np.expand_dims(image, axis=0)  # Ajoutez une dimension batch
    
    predictions = model.predict(image)
    
    # Assurez-vous que `predictions` a la forme correcte
    if predictions.shape[1] == 1001:
        predictions = predictions[:, 1:]  # Supprimez la première colonne si elle est redondante
    
    decoded_predictions = tf.keras.applications.mobilenet_v2.decode_predictions(predictions, top=3)[0]
    return os.path.basename(image_path), decoded_predictions

# Fonction d'inférence séquentielle sur toutes les images prétraitées
def process_images_sequential(output_sequentiel, model):
    image_paths = [os.path.join(output_sequentiel, filename) for filename in os.listdir(output_sequentiel) if filename.endswith('.npy')]
    
    start_time = time.time()  # Début de la mesure du temps
    for image_path in image_paths:
        process_image_with_model(image_path, model)
    end_time = time.time()  # Fin de la mesure du temps
    
    return end_time - start_time

# Set CPU affinity to one core
p = psutil.Process(os.getpid())
p.cpu_affinity([0])  # Use only the first core

# Exécution du prétraitement et de l'inférence séquentiels
preprocessing_time = preprocess_images_sequential(input_sequentiel, output_sequentiel)
inference_time = process_images_sequential(output_sequentiel, model)

# Mesure des métriques système
cpu_usage, temperature = get_system_metrics()
num_cores_used = len(p.cpu_affinity())

# Affichage des résultats
print(f"Sequential preprocessing time: {preprocessing_time:.2f} seconds")
print(f"Sequential inference time: {inference_time:.2f} seconds")
print(f"CPU Usage: {cpu_usage}%")
print(f"Temperature: {temperature}°C")
print(f"Number of cores used: {num_cores_used}")
