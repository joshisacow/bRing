import pandas as pd
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('/Users/bryantchung/Duke/Senior Year/CS 655/esp/esp-idf/Labs/Lab5_AWS_Database/sensor_data.csv')

# Convert timestamp to datetime if needed
data['timestamp'] = pd.to_datetime(data['timestamp'])

# Plot temperature and humidity over time
plt.figure(figsize=(12, 6))
plt.plot(data['timestamp'], data['temp'], label='Temperature (°C)', color='red')
plt.plot(data['timestamp'], data['humidity'], label='Humidity (%)', color='blue')
plt.xlabel('Timestamp')
plt.ylabel('Values')
plt.title('Temperature and Humidity over 24 Hours')
plt.legend()
plt.savefig('temperature_humidity_plot.png')
plt.show()

