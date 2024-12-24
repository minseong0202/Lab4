#include <gtk/gtk.h>
#include <string.h>

GtkWidget *input_entry;
char display_text[256] = "";

void on_button_pressed(GtkWidget *widget, gpointer data) {
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    if (strcmp(button_label, "C") == 0) {
        display_text[0] = '\0';
    } else if (strcmp(button_label, "=") == 0) {
        double result = 0;
        char operation;
        double operand1, operand2;

        if (sscanf(display_text, "%lf %c %lf", &operand1, &operation, &operand2) == 3) {
            switch (operation) {
                case '+': result = operand1 + operand2; break;
                case '-': result = operand1 - operand2; break;
                case '*': result = operand1 * operand2; break;
                case '/': result = (operand2 != 0) ? operand1 / operand2 : 0; break;
                default: strcpy(display_text, "Error"); break;
            }
            snprintf(display_text, sizeof(display_text), "%g", result);
        } else {
            strcpy(display_text, "Error");
        }
    } else {
        strcat(display_text, button_label);
    }

    gtk_entry_set_text(GTK_ENTRY(input_entry), display_text);
}

void initialize_application(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *layout;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 300);

    layout = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), layout);

    input_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(layout), input_entry, 0, 0, 4, 1);

    const char *button_labels[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "C", "0", "=", "+"
    };

    int index = 0;
    for (int row = 1; row <= 4; row++) {
        for (int col = 0; col < 4; col++) {
            GtkWidget *button = gtk_button_new_with_label(button_labels[index]);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_pressed), NULL);
            gtk_grid_attach(GTK_GRID(layout), button, col, row, 1, 1);
            index++;
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.calculator", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(initialize_application), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
