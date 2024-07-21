#include <gtk/gtk.h>
#include <string>
#include <stack>
#include <sstream>

// Utility function to evaluate the arithmetic expression
double evaluate_expression(const std::string &expression) {
    std::istringstream iss(expression);
    std::stack<double> values;
    std::stack<char> operators;
    double number;
    char token;

    while (iss >> token) {
        if (isdigit(token) || token == '.') {
            iss.putback(token);
            iss >> number;
            values.push(number);
        } else if (token == '(') {
            operators.push(token);
        } else if (token == ')') {
            while (!operators.empty() && operators.top() != '(') {
                char op = operators.top();
                operators.pop();

                double val2 = values.top();
                values.pop();
                double val1 = values.top();
                values.pop();

                switch (op) {
                    case '+': values.push(val1 + val2); break;
                    case '-': values.push(val1 - val2); break;
                    case '*': values.push(val1 * val2); break;
                    case '/': values.push(val1 / val2); break;
                }
            }
            if (!operators.empty()) {
                operators.pop();
            }
        } else {
            while (!operators.empty() && operators.top() != '(' &&
                   ((token == '+' || token == '-') || (token == '*' || token == '/'))) {
                char op = operators.top();
                operators.pop();

                double val2 = values.top();
                values.pop();
                double val1 = values.top();
                values.pop();

                switch (op) {
                    case '+': values.push(val1 + val2); break;
                    case '-': values.push(val1 - val2); break;
                    case '*': values.push(val1 * val2); break;
                    case '/': values.push(val1 / val2); break;
                }
            }
            operators.push(token);
        }
    }

    while (!operators.empty()) {
        char op = operators.top();
        operators.pop();

        double val2 = values.top();
        values.pop();
        double val1 = values.top();
        values.pop();

        switch (op) {
            case '+': values.push(val1 + val2); break;
            case '-': values.push(val1 - val2); break;
            case '*': values.push(val1 * val2); break;
            case '/': values.push(val1 / val2); break;
        }
    }

    return values.top();
}

// Callback functions for button clicks
static void on_number_clicked(GtkButton *button, gpointer data) {
    const gchar *num = gtk_button_get_label(button);
    GtkEntry *entry = GTK_ENTRY(data);
    const gchar *current_text = gtk_entry_get_text(entry);
    gchar *new_text = g_strdup_printf("%s%s", current_text, num);
    gtk_entry_set_text(entry, new_text);
    g_free(new_text);
}

static void on_clear_clicked(GtkButton *button, gpointer data) {
    GtkEntry *entry = GTK_ENTRY(data);
    gtk_entry_set_text(entry, "");
}

static void on_equal_clicked(GtkButton *button, gpointer data) {
    GtkEntry *entry = GTK_ENTRY(data);
    const gchar *expression = gtk_entry_get_text(entry);

    try {
        double result = evaluate_expression(expression);
        gchar *result_text;

        // Check if the result is an integer
        if (result == (int)result) {
            result_text = g_strdup_printf("%d", (int)result);
        } else {
            result_text = g_strdup_printf("%f", result);
        }

        gtk_entry_set_text(entry, result_text);
        g_free(result_text);
    } catch (const std::exception &e) {
        gtk_entry_set_text(entry, "Error");
    }
}
//The main function will be started up and will allow us to create the application. 
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Fast Basic Calculator 1.0");
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "icon.png", NULL);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE); //You wont be able to resize the window
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);
    gtk_widget_set_size_request(entry, 250, 70); // Width: 250, Height: 40
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "entry { font-size: 30px; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(entry);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    const gchar *button_labels[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "C", "=", "+"}
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            GtkWidget *button = gtk_button_new_with_label(button_labels[i][j]);
            gtk_widget_set_size_request(button, 100, 150); //Changing the button size
            GtkCssProvider *provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(provider,
            "button { font-size: 60px; }", -1, NULL);
            GtkStyleContext *context = gtk_widget_get_style_context(button);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            gtk_grid_attach(GTK_GRID(grid), button, j, i + 1, 1, 1);

            if (g_strcmp0(button_labels[i][j], "C") == 0) {
                g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), entry);
            } else if (g_strcmp0(button_labels[i][j], "=") == 0) {
                g_signal_connect(button, "clicked", G_CALLBACK(on_equal_clicked), entry);
            } else {
                g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), entry);
            }
        }
    }



    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
