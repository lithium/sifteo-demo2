


class SineString {
public:
    static const uint8_t MAX_LETTERS=8;

    Float2 pos;
    const char *letters;
    unsigned letter_size;
    unsigned n_letters;


    Float2 letter_pos[MAX_LETTERS];
    unsigned letter_ofs[MAX_LETTERS];
    float angle;

    const float hh = (M_2_PI/100.0)*2;
    const float spd = 0.07;
    const float letter_spd = 0.4;
    const float scroll_spd = 0.9;
    const float amplitude = 28;


    void init(Float2 topLeft, const char *string, unsigned padding=7)
    {
        letters = string;
        for (letter_size=0; string[letter_size]; letter_size++)
            ;
        n_letters = MIN(letter_size,MAX_LETTERS);
        pos = topLeft;
        angle = spd;
        for (unsigned i=0; i < n_letters; i++) {
            letter_pos[i].x = pos.x + i*(8+padding);
            letter_ofs[i] = i;
        }

        tick();
    }

    void paint(VideoBuffer *vbuf)
    {
        float h = angle;
        for (unsigned i=0; i < n_letters; i++) {
            unsigned frame = letters[letter_ofs[i]] - ' ';
            vbuf->sprites[i].setImage(Font, frame);
            vbuf->sprites[i].move(letter_pos[i]);

        }
    }

    void tick()
    {
        angle += spd;

        float h = angle;
        for (unsigned i=0; i < n_letters; i++) {
            letter_pos[i].x -= scroll_spd;
            letter_pos[i].y = pos.y + tsin(h)*amplitude;
            h += letter_spd + hh;

            if (letter_pos[i].x < 0) {
                letter_ofs[i] = (letter_ofs[i] + n_letters) % letter_size;
                letter_pos[i].x = 120;
            }
        }
    }

};

class DemoCube {
public:
    CubeID id;
    VideoBuffer vbuf;

    void init(CubeID id)
    {
        this->id = id;
        vbuf.attach(this->id);
        vbuf.initMode(BG0_SPR_BG1);
    }
    void loading(unsigned progress)
    {
        vbuf.initMode(BG0_ROM);
        vbuf.bg0rom.hBargraph(vec(0,16), progress, BG0ROMDrawable::BLUE);
    }
    void paint()
    {
        vbuf.bg0.image(vec(0,0), Background);
    }
};

class Demo {
public:
    AssetLoader loader;
    AssetConfiguration<1> config;


    bool running;
    DemoCube cubes[CUBE_ALLOCATION];
    CubeSet cubes_new, cubes_active;


    SineString string;


    Demo() : running(true) 
    {
        config.append(MainSlot, DemoAssets);
        loader.init();
    }

    void init()
    {


        cubes_new = CubeSet::connected();

        string.init(vec(40, 56), "WRITTEN BY LiTHiUM (2012)...       MUSIC BY MQ4 (ASTRAL LIFE)...         SIFTEOS!                  ");

        Events::cubeConnect.set(&Demo::onCubeConnect, this);
        Events::cubeDisconnect.set(&Demo::onCubeDisconnect, this);
        Events::gameMenu.set(&Demo::onRestart, this, "Restart");

    }

    void cleanup()
    {
       AudioTracker::stop();
       Events::cubeConnect.unset();
       Events::cubeDisconnect.unset();
       Events::gameMenu.unset();

   }

    void run()
    {
        AudioTracker::play(Music);

        TimeStep ts;
        running=true;
        while (running) {


            if (!cubes_new.empty()) {
                AudioTracker::pause();
                loader.start(config);
                while (!loader.isComplete()) {
                    for (CubeID cid : cubes_new) {
                        cubes[cid].loading(loader.cubeProgress(cid, 128));
                    }
                    System::paint();
                }
                loader.finish();
                for (CubeID cid : cubes_new) {
                    cubes[cid].init(cid);
                    cubes_active.mark(cid);
                }
                AudioTracker::resume();
                cubes_new.clear(); 
            }



            ts.next();
            tick(ts.delta());
            paint();
            System::paint();
        }
    }

    void paint()
    {
        for (CubeID cid : cubes_active) {
            cubes[cid].paint();

            string.paint(&(cubes[cid].vbuf));
        }
    }

    void tick(TimeDelta step)
    {
        string.tick(); 
    }

    void onCubeConnect(unsigned cid)
    {
        cubes_new.mark(cid);
        cubes[cid].loading(0);
    }
    void onCubeDisconnect(unsigned cid) {
        cubes_new.clear(cid);
        cubes_active.clear(cid);
    }
    void onRestart() {
        running = false;
    }

};
