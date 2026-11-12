#include <gtkmm.h>
#include <glibmm/main.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <chrono>

#include "widget.hpp"

class NetSpeedWidget : public PanelWidget
{
private:
    struct Settings
    {
        std::string down_color = "#22c55e";
        std::string up_color = "#60a5fa";
        std::string units = "auto";
        std::string display = "both";
        int font_size = 10;
        int interval = 1000;
    } settings;

    Gtk::Label *label = nullptr;
    Gtk::EventBox *event_box = nullptr;
    Gtk::Menu context_menu;
    Gtk::MenuItem settings_item;

    sigc::connection timer;

    std::string interface_name;
    unsigned long long last_rx = 0;
    unsigned long long last_tx = 0;
    double last_down = 0;
    double last_up = 0;

    std::chrono::steady_clock::time_point last_update;

    std::string config_path()
    {
        const char *home = std::getenv("HOME");
        return std::string(home ? home : "") +
               "/.config/netspeed-widget.conf";
    }

    void load_settings()
    {
        std::ifstream f(config_path());
        std::string line;

        while (std::getline(f, line))
        {
            auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (key == "down_color") settings.down_color = value;
            if (key == "up_color") settings.up_color = value;
            if (key == "units") settings.units = value;
            if (key == "display") settings.display = value;
            if (key == "font_size") settings.font_size = std::stoi(value);
            if (key == "interval") settings.interval = std::stoi(value);
        }

        settings.font_size = std::clamp(settings.font_size, 8, 18);
        settings.interval = std::clamp(settings.interval, 500, 5000);
    }

    void save_settings()
    {
        std::ofstream f(config_path());

        f << "down_color=" << settings.down_color << "\n";
        f << "up_color=" << settings.up_color << "\n";
        f << "units=" << settings.units << "\n";
        f << "display=" << settings.display << "\n";
        f << "font_size=" << settings.font_size << "\n";
        f << "interval=" << settings.interval << "\n";
    }

    std::string color_hex(const Gdk::RGBA& color)
    {
        std::ostringstream s;
        s << "#"
          << std::hex << std::setfill('0')
          << std::setw(2) << (int)(color.get_red() * 255)
          << std::setw(2) << (int)(color.get_green() * 255)
          << std::setw(2) << (int)(color.get_blue() * 255);
        return s.str();
    }

    unsigned long long counter(const std::string& type)
    {
        std::ifstream f("/sys/class/net/" + interface_name +
                        "/statistics/" + type);
        unsigned long long value = 0;
        f >> value;
        return value;
    }

    std::string find_interface()
    {
        FILE *pipe = popen(
            "ip -4 route get 1.1.1.1 2>/dev/null", "r");

        std::string result;

        if (pipe)
        {
            char buffer[512];

            if (fgets(buffer, sizeof(buffer), pipe))
            {
                std::istringstream route(buffer);
                std::string token, previous;

                while (route >> token)
                {
                    if (previous == "dev")
                    {
                        result = token;
                        break;
                    }
                    previous = token;
                }
            }

            pclose(pipe);
        }

        return result;
    }

    std::string format_speed(double value)
    {
        std::ostringstream s;
        s << std::fixed << std::setprecision(2);

        if (settings.units == "mbps")
            s << value * 8.0 / 1000000.0 << " Mbps";
        else if (settings.units == "kb")
            s << value / 1024.0 << " KB/s";
        else if (settings.units == "mb")
            s << value / 1048576.0 << " MB/s";
        else if (value >= 1048576.0)
            s << value / 1048576.0 << " MB/s";
        else
            s << value / 1024.0 << " KB/s";

        return s.str();
    }

    void render(double down, double up)
    {
        if (!label)
            return;

        std::string size = std::to_string(settings.font_size * 1024);

        std::string markup;

        if (settings.display == "both" ||
            settings.display == "down")
        {
            markup += "<span foreground='" + settings.down_color +
                      "' size='" + size + "'>↓ " +
                      format_speed(down) + "</span>";
        }

        if (settings.display == "both")
            markup += "\n";

        if (settings.display == "both" ||
            settings.display == "up")
        {
            markup += "<span foreground='" + settings.up_color +
                      "' size='" + size + "'>↑ " +
                      format_speed(up) + "</span>";
        }

        label->set_markup(markup);
    }

    bool update()
    {
        std::string current = find_interface();
        auto now = std::chrono::steady_clock::now();

        if (current.empty())
        {
            interface_name.clear();
            last_rx = last_tx = 0;
            last_down = last_up = 0;
            last_update = now;
            render(0, 0);
            return true;
        }

        if (current != interface_name)
        {
            interface_name = current;
            last_rx = counter("rx_bytes");
            last_tx = counter("tx_bytes");
            last_update = now;
            last_down = last_up = 0;
            render(0, 0);
            return true;
        }

        unsigned long long rx = counter("rx_bytes");
        unsigned long long tx = counter("tx_bytes");

        double seconds =
            std::chrono::duration<double>(now - last_update).count();

        if (seconds > 0)
        {
            last_down = rx >= last_rx ?
                (rx - last_rx) / seconds : 0;

            last_up = tx >= last_tx ?
                (tx - last_tx) / seconds : 0;
        }

        last_rx = rx;
        last_tx = tx;
        last_update = now;

        render(last_down, last_up);

        return true;
    }

    void show_settings()
    {
        Gtk::Dialog dialog("Impostazioni NetSpeed", true);

        dialog.add_button("_Ripristina predefiniti", 1001);
        dialog.add_button("_Annulla", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Salva", Gtk::RESPONSE_OK);

        Gtk::Grid grid;
        grid.set_row_spacing(10);
        grid.set_column_spacing(12);
        grid.set_margin_start(12);
        grid.set_margin_end(12);
        grid.set_margin_top(12);
        grid.set_margin_bottom(12);

        Gtk::Label down_label("Colore download:");
        Gtk::Label up_label("Colore upload:");
        Gtk::Label font_label("Dimensione carattere:");
        Gtk::Label units_label("Unità di misura:");
        Gtk::Label display_label("Visualizzazione:");
        Gtk::Label interval_label("Aggiornamento (ms):");

        Gtk::ColorButton down_color;
        Gtk::ColorButton up_color;

        Gdk::RGBA c;
        c.set(settings.down_color);
        down_color.set_rgba(c);

        c.set(settings.up_color);
        up_color.set_rgba(c);

        Gtk::SpinButton font_size;
        font_size.set_range(8, 18);
        font_size.set_increments(1, 1);
        font_size.set_value(settings.font_size);

        Gtk::SpinButton interval;
        interval.set_range(500, 5000);
        interval.set_increments(500, 500);
        interval.set_value(settings.interval);

        Gtk::ComboBoxText units;
        units.append("auto", "Automatico (KB/s - MB/s)");
        units.append("kb", "KB/s");
        units.append("mb", "MB/s");
        units.append("mbps", "Mbps");
        units.set_active_id(settings.units);

        Gtk::ComboBoxText display;
        display.append("both", "Download + Upload");
        display.append("down", "Solo Download");
        display.append("up", "Solo Upload");
        display.set_active_id(settings.display);

        grid.attach(down_label, 0, 0, 1, 1);
        grid.attach(down_color, 1, 0, 1, 1);

        grid.attach(up_label, 0, 1, 1, 1);
        grid.attach(up_color, 1, 1, 1, 1);

        grid.attach(font_label, 0, 2, 1, 1);
        grid.attach(font_size, 1, 2, 1, 1);

        grid.attach(units_label, 0, 3, 1, 1);
        grid.attach(units, 1, 3, 1, 1);

        grid.attach(display_label, 0, 4, 1, 1);
        grid.attach(display, 1, 4, 1, 1);

        grid.attach(interval_label, 0, 5, 1, 1);
        grid.attach(interval, 1, 5, 1, 1);

        dialog.get_content_area()->pack_start(grid);
        grid.show_all();

        int response = dialog.run();

        if (response == 1001)
        {
            settings = Settings{};
            save_settings();

            timer.disconnect();
            timer = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &NetSpeedWidget::update),
                settings.interval);

            render(last_down, last_up);
            return;
        }

        if (response == Gtk::RESPONSE_OK)
        {
            settings.down_color = color_hex(down_color.get_rgba());
            settings.up_color = color_hex(up_color.get_rgba());
            settings.font_size = font_size.get_value_as_int();
            settings.interval = interval.get_value_as_int();

            settings.units = units.get_active_id().raw();
            settings.display = display.get_active_id().raw();

            save_settings();

            timer.disconnect();
            timer = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &NetSpeedWidget::update),
                settings.interval);

            render(last_down, last_up);
        }
    }

    bool on_button_press(GdkEventButton *event)
    {
        if (event->type == GDK_BUTTON_PRESS && event->button == 3)
        {
            context_menu.popup(event->button, event->time);
            return true;
        }

        return false;
    }

public:
    NetSpeedWidget(int)
    {
        widget_name = "netspeed";

        load_settings();

        settings_item.set_label("Impostazioni NetSpeed");
        context_menu.append(settings_item);
        settings_item.show();

        settings_item.signal_activate().connect(
            sigc::mem_fun(*this, &NetSpeedWidget::show_settings));
    }

    void widget_init(Gtk::HBox *box) override
    {
        label = Gtk::manage(new Gtk::Label());

        label->set_use_markup(true);
        label->set_text("↓ 0 KB/s  ↑ 0 KB/s");

        event_box = Gtk::manage(new Gtk::EventBox());
        event_box->set_visible_window(false);
        event_box->add_events(Gdk::BUTTON_PRESS_MASK);
        event_box->add(*label);

        event_box->signal_button_press_event().connect(
            sigc::mem_fun(*this, &NetSpeedWidget::on_button_press),
            false);

        box->pack_start(*event_box, Gtk::PACK_SHRINK);

        event_box->show_all();

        interface_name = find_interface();

        if (!interface_name.empty())
        {
            last_rx = counter("rx_bytes");
            last_tx = counter("tx_bytes");
        }

        last_update = std::chrono::steady_clock::now();

        timer = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &NetSpeedWidget::update),
            settings.interval);
    }

    ~NetSpeedWidget()
    {
        timer.disconnect();
    }
};

extern "C" PanelWidget *create(int value)
{
    return new NetSpeedWidget(value);
}
