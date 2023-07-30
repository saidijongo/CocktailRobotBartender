import tkinter as tk
from PIL import ImageTk, Image
import json
import requests
import RPi.GPIO as GPIO
import time
from io import BytesIO
from tkinter import ttk

# Defining the GPIO pins connected to the relay module
# relay_pins = [23, 21, 19, 15, 13, 11, 7, 40, 38, 36, 32, 37, 35, 33, 31, 29]
relay_pins = [23, 21, 19, 15, 13, 11, 7, 40, 38, 36, 32, 37, 35, 33, 31, 29]

# Loading recipes from JSON
with open('recipes.json') as file:
    recipes = json.load(file)


class CocktailBartenderRobotGUI:
    def __init__(self, root, recipes):
        self.root = root
        self.recipes = recipes
        self.cocktail_images = []
        self.cocktail_names = []

        # Initializing GPIO setup here
        GPIO.setmode(GPIO.BOARD)
        GPIO.setwarnings(False)
        for pin in relay_pins:
            GPIO.setup(pin, GPIO.OUT)
            GPIO.output(pin, GPIO.HIGH)

        self.jongo = tk.Frame(self.root, bg="#F8C471")
        self.jongo.pack(fill=tk.BOTH, expand=1)

        self.canva = tk.Canvas(self.jongo)
        self.canva.pack(side=tk.LEFT, fill=tk.BOTH, expand=1)

        self.v_scrollbar = ttk.Scrollbar(
            self.jongo, orient=tk.VERTICAL, command=self.canva.yview)
        self.v_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.h_scrollbar = ttk.Scrollbar(
            self.jongo, orient=tk.HORIZONTAL, command=self.canva.xview)
        self.h_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)

        self.canva.configure(yscrollcommand=self.v_scrollbar.set,
                             xscrollcommand=self.h_scrollbar.set)
        self.canva.bind("<Configure>", lambda e: self.canva.configure(
            scrollregion=self.canva.bbox("all")))

        self.jongo2 = tk.Frame(self.canva, bg="#F8C471")
        self.canva.create_window((0, 0), window=self.jongo2, anchor="nw")

        self.load_cocktail_data()
        self.create_cocktail_buttons()

    def load_cocktail_data(self):
        for cocktail in self.recipes:
            self.cocktail_names.append(cocktail)
            response = requests.get(self.recipes[cocktail]['image_url'])
            try:
                image = Image.open(BytesIO(response.content))
                image = image.resize((145, 145), Image.ANTIALIAS)
                self.cocktail_images.append(ImageTk.PhotoImage(image))
            except Exception as e:
                print(f"Error loading image for {cocktail}: {e}")
                # Set a default image or handle the error accordingly
                self.cocktail_images.append(None)

    def create_cocktail_buttons(self):
        for i, cocktail in enumerate(self.cocktail_names):
            btn_frame = tk.Frame(self.jongo2, bg="#F8C471")
            btn_frame.grid(row=i // 4, column=i % 4, padx=10, pady=10)

            btn = tk.Button(btn_frame, image=self.cocktail_images[i],
                            command=lambda idx=i: self.show_cocktail_details(idx))
            btn.pack()

            label = tk.Label(btn_frame, text=cocktail)
            label.pack()

    def show_cocktail_details(self, idx):
        selected_cocktail = self.cocktail_names[idx]
        cocktail_data = self.recipes[selected_cocktail]

        details_window = tk.Toplevel(self.root, bg="#F8C471")
        details_window.title(selected_cocktail)
        details_window.geometry("250x470")

        # Displaying cocktail image
        image_frame = tk.Frame(details_window, bg="#F8C471")
        image_frame.pack(pady=10)
        image_label = tk.Label(image_frame, image=self.cocktail_images[idx])
        image_label.pack()

        # Displaying cocktail ingredients
        ingredients_frame = tk.Frame(details_window, bg="#F8C471")
        ingredients_frame.pack(pady=10)
        for ingredient in cocktail_data['ingredients']:
            ingredient_label = tk.Label(
                ingredients_frame, text=f"{ingredient['name']}: {ingredient['quantity']} ml", bg="#F8C471")
            ingredient_label.pack()

        # Adding number of cocktails to cart
        cart_frame = tk.Frame(details_window, bg="#F8C471")
        cart_frame.pack(pady=10)
        cart_label = tk.Label(cart_frame, text="Slide->", bg="#F8C471")
        cart_label.pack(side=tk.LEFT)

        cart_value = tk.IntVar(value=1)  # default cocktail count to one
        cart_slider = ttk.Scale(
            cart_frame, from_=1, to=10, variable=cart_value, orient=tk.HORIZONTAL)
        cart_slider.pack(side=tk.LEFT)

        count_label = tk.Label(
            cart_frame, text=f"Order: {cart_value.get()}", bg="#F8C471")
        count_label.pack(pady=5)

        # Update the count label when the slider value changes
        def update_cocktail_count(event):
            count_label.configure(text=f"Order: {cart_value.get()}")

        cart_slider.bind("<ButtonRelease-1>", update_cocktail_count)

        # Order button
        def order_cocktails():
            num_cocktails = cart_value.get()
            self.make_cocktails(selected_cocktail, num_cocktails)
            details_window.destroy()

        order_button = tk.Button(
            details_window, text="Press to order", command=order_cocktails)
        order_button.pack(pady=10)

    def make_cocktails(self, cocktail_name, num_cocktails):
        selected_cocktail = self.recipes[cocktail_name]
        print(f"Preparing {num_cocktails} {cocktail_name}(s)...")

        # Getting the ingredient motors and volumes for the selected cocktail
        ingredients = selected_cocktail['ingredients']

        # Calculating the estimated pump run times based on ingredient volumes
        run_times = []
        for ingredient in ingredients:
            volume = ingredient['quantity']
            motor_pin = relay_pins[ingredient['motor']]
            run_time = volume / 100
            run_times.append((motor_pin, run_time))

        # Activating all relays for the selected cocktails at the same time
        for _ in range(num_cocktails):
            for motor_pin, _ in run_times:
                self.turn_on_relay(motor_pin)

        # Waiting for the longest run time (time for the last motor to stop)
        longest_run_time = max(run_time for _, run_time in run_times)
        time.sleep(longest_run_time)

        # Turning off relays for each motor based on their individual run times
        for motor_pin, run_time in run_times:
            self.turn_off_relay(motor_pin)
            time.sleep(run_time)

        print(f"{num_cocktails} {cocktail_name}(s) are ready!")

    def turn_on_relay(self, pin):
        GPIO.output(pin, GPIO.LOW)

    def turn_off_relay(self, pin):
        GPIO.output(pin, GPIO.HIGH)


if __name__ == "__main__":
    # Creating a tkinter root window
    root = tk.Tk()
    root.title("Cocktail Bartender Robot, CBR")
    root.configure(bg="#F8C471")
    root.geometry("1200x720")

    gui = CocktailBartenderRobotGUI(root, recipes)
    root.mainloop()

    # Cleaning up GPIO
    GPIO.cleanup()
