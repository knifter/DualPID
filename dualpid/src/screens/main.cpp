#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "inputdrv.h"
#include "tools-log.h"
#include "globals.h"
#include "rtc.h"

/*********************************************************************************************************************************/
class PidPanel
{
	public:
		typedef enum{
			PS_DISABLED,
			PS_OK,
		} state_t;

		PidPanel(lv_obj_t* parent, const uint8_t num, PIDLoop& pid_in);

        int num;
		PIDLoop& pid;
		lv_obj_t	*box, *lbl_sp, *lbl_value, *bar_output;
		lv_style_t 	style_font26;
        const char *unit;
        uint8_t prec;
		lv_style_t style_indic;
        PIDLoop::status_t current_status;

		const void update();
		const void setSelected(bool);
		const void setState(PIDLoop::status_t s);
};

PidPanel::PidPanel(lv_obj_t* parent, const uint8_t num_in, PIDLoop& pid_in)
	 : num(num_in), pid(pid_in)
{
	unit = "U";
	prec = 0;
	lv_color_t color1 = COLOR_BLACK;
    InputDriver* indrv = pid.input_drv();
	unit =   indrv->unit_text();
	prec =   indrv->precision();
	color1 = indrv->color1();
	lv_color_t color2 = lv_color_lighten(color1, 4);

	box = lv_obj_create(parent);

	lv_obj_set_size(box, DISPLAY_WIDTH/2, 80);
	lv_obj_set_style_border_width(box, 3, 0);
	lv_obj_set_style_pad_all(box, 5, 0);
    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);

	lv_style_init(&style_font26);
	lv_style_set_text_font(&style_font26, &lv_font_montserrat_26);

	// Setpoint label
	{
		lbl_sp = lv_label_create(box);
		lv_obj_set_style_border_width(lbl_sp, 1, 0);
		lv_obj_set_size(lbl_sp, LV_PCT(100), 20);
		lv_obj_align(lbl_sp, LV_ALIGN_TOP_MID, 0, 0);
		// lv_obj_set_style_text_color(lbl_sp, COLOR_GREY, LV_PART_ANY);
		lv_obj_set_style_text_align(lbl_sp, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(lbl_sp, "<setpoint>");
		lv_obj_set_style_bg_opa(lbl_sp, LV_OPA_COVER, 0);
		lv_obj_set_style_bg_color(lbl_sp, color2, 0);
	};
	// Value label
	{
		lbl_value = lv_label_create(box);
		lv_obj_set_style_border_width(lbl_value, 1, 0);
		lv_obj_set_size(lbl_value, LV_PCT(100), 30);
		lv_obj_align_to(lbl_value, lbl_sp, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
		lv_obj_add_style(lbl_value, &style_font26, 0);
		lv_obj_set_style_text_color(lbl_value, COLOR_BLACK, LV_PART_ANY);
		lv_obj_set_style_text_align(lbl_value, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(lbl_value, "<value>");
		lv_obj_set_style_bg_opa(lbl_value, LV_OPA_COVER, 0);
	};

	// Output bar
	{
		bar_output = lv_bar_create(box);
		lv_obj_set_style_border_width(bar_output, 1, 0);
		lv_obj_set_size(bar_output, LV_PCT(100), 10);
		lv_obj_align(bar_output, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_bar_set_range(bar_output, 0, 100);
	    lv_obj_set_style_bg_color(bar_output, COLOR_WHITE, 0);
		lv_obj_set_style_bg_opa(bar_output, LV_OPA_COVER, 0);

    	lv_style_init(&style_indic);
    	lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    	lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);
        lv_style_set_bg_color(&style_indic, COLOR_BLUE);
        lv_style_set_bg_grad_color(&style_indic, COLOR_RED);

		lv_obj_add_style(bar_output, &style_indic, LV_PART_INDICATOR);
	};

    // Set all state properties, but to trigger we need to set status to something we're not
    current_status = PIDLoop::STATUS_NONE;
    setState(PIDLoop::STATUS_INACTIVE);
    // current_status => disabled
}; // PidPanel()

const void PidPanel::setState(PIDLoop::status_t status)
{
    // Only update widget styles if status changed
    if(current_status == status)
        return;
    current_status = status;

	// Disabled: Hide it all
	switch(status)
	{
		case PIDLoop::STATUS_NONE:
		case PIDLoop::STATUS_DISABLED:	// TODO: disable widget completely
            // lv_obj_add_flag(lbl_sp, LV_OBJ_FLAG_HIDDEN);
            // lv_obj_add_flag(lbl_value, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(box, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(bar_output, LV_OBJ_FLAG_HIDDEN);
			return;
		default:
			break;
	};

    // Setpoint label
    switch(status)
	{
		case PIDLoop::STATUS_NONE:
		case PIDLoop::STATUS_DISABLED:	// TODO: disable widget completely
			break;
		case PIDLoop::STATUS_SENSOR:

			// lv_obj_set_style_bg_color(lbl_sp, COLOR_LIGHT_BLUE, 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_GREY_DARK(1), 0);
			break;
		case PIDLoop::STATUS_INACTIVE:
			// lv_obj_set_style_bg_color(lbl_sp, COLOR_BLUE_GREY, 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_BLACK, 0);
            break;
		case PIDLoop::STATUS_ERROR:
			// lv_obj_set_style_bg_color(lbl_sp, COLOR_RED_LIGHT(2), 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_BLACK, 0);
            break;
		case PIDLoop::STATUS_UNLOCKED:
			// lv_obj_set_style_bg_color(lbl_sp, COLOR_GREEN_LIGHT(2), 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_BLACK, 0);
            break;
		case PIDLoop::STATUS_LOCKED_WAIT:
			// lv_obj_set_style_bg_color(lbl_sp, COLOR_GREEN_LIGHT(2), 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_BLACK, 0);
            break;
		case PIDLoop::STATUS_LOCKED:
			// lv_obj_set_style_bg_color(lbl_sp, COLOR_GREEN_LIGHT(2), 0);
            lv_obj_set_style_text_color(lbl_sp, COLOR_BLACK, 0);
			break;
		case PIDLoop::STATUS_FIXED:
			lv_obj_set_style_text_color(lbl_sp, COLOR_PURPLE, 0);
			break;
    };

    // Value label, Background
    switch(status)
	{
		case PIDLoop::STATUS_NONE:
		case PIDLoop::STATUS_DISABLED:	// TODO: disable widget completely
			break;
		case PIDLoop::STATUS_SENSOR:
			lv_obj_set_style_bg_color(lbl_value, COLOR_WHITE, 0); 
		    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
		    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);
            break;
		case PIDLoop::STATUS_INACTIVE:
			lv_obj_set_style_bg_color(lbl_value, COLOR_GREY_LIGHT(2), 0);
		    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
		    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);
            break;
		case PIDLoop::STATUS_ERROR:
			lv_obj_set_style_bg_color(lbl_value, COLOR_RED, 0);
		    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
		    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);
            break;
		case PIDLoop::STATUS_UNLOCKED:
			lv_obj_set_style_bg_color(lbl_value, COLOR_ORANGE_LIGHT(2), 0);
		    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
		    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);
            break;
		case PIDLoop::STATUS_LOCKED_WAIT:
			lv_obj_set_style_bg_color(lbl_value, COLOR_GREEN_LIGHT(2), 0);
		    lv_obj_set_style_border_color(box, COLOR_WHITE, 0);
		    lv_obj_set_style_bg_color(box, COLOR_WHITE, 0);
			break;
		case PIDLoop::STATUS_LOCKED:
			lv_obj_set_style_bg_color(lbl_value, COLOR_GREEN_LIGHT(2), 0);
		    lv_obj_set_style_border_color(box, COLOR_GREEN_LIGHT(2), 0);
		    lv_obj_set_style_bg_color(box, COLOR_GREEN_LIGHT(2), 0);
			break;
		case PIDLoop::STATUS_FIXED:
			lv_obj_set_style_bg_color(lbl_value, COLOR_PURPLE_LIGHT(2), 0); 
		    lv_obj_set_style_border_color(box, COLOR_PURPLE_LIGHT(2), 0);
		    lv_obj_set_style_bg_color(box, COLOR_PURPLE_LIGHT(2), 0);
			break;
    };

    // Output bar
    switch(status)
    {
		case PIDLoop::STATUS_NONE:
		case PIDLoop::STATUS_DISABLED:	// TODO: disable widget completely
			break;
        case PIDLoop::STATUS_SENSOR:
            lv_obj_add_flag(bar_output, LV_OBJ_FLAG_HIDDEN);
            break;
        case PIDLoop::STATUS_INACTIVE:
            lv_obj_clear_flag(bar_output, LV_OBJ_FLAG_HIDDEN);

			// lv_obj_set_style_bg_color(bar_output, COLOR_WHITE, 0);
   			lv_style_set_bg_color(&style_indic, COLOR_GREY_LIGHT(1));
			lv_style_set_bg_grad_color(&style_indic, COLOR_GREY_LIGHT(1));
            break;
		case PIDLoop::STATUS_ERROR:
		case PIDLoop::STATUS_UNLOCKED:
        case PIDLoop::STATUS_LOCKED_WAIT:
        case PIDLoop::STATUS_LOCKED:
            lv_obj_clear_flag(bar_output, LV_OBJ_FLAG_HIDDEN);

			// lv_obj_set_style_bg_color(bar_output, COLOR_WHITE, 0);
            lv_style_set_bg_color(&style_indic, COLOR_BLUE);
            lv_style_set_bg_grad_color(&style_indic, COLOR_RED);
            break;
		case PIDLoop::STATUS_FIXED:
            lv_obj_clear_flag(bar_output, LV_OBJ_FLAG_HIDDEN);

			// lv_obj_set_style_bg_color(bar_output, COLOR_WHITE, 0);
            lv_style_set_bg_color(&style_indic, COLOR_PURPLE);
            lv_style_set_bg_grad_color(&style_indic, COLOR_PURPLE);
			break;
    };

};
const void PidPanel::update()
{ 
	// panel status/color
	setState(pid.status());

	// top barbox
    switch(current_status)
    {
        case PIDLoop::STATUS_SENSOR:
		case PIDLoop::STATUS_NONE:
            lv_label_set_text(lbl_sp, "-");
            break;
		case PIDLoop::STATUS_FIXED:
            lv_label_set_text_fmt(lbl_sp, "out = %u%%", pid._settings.fixed_output_value);
			break;
        default:
            lv_label_set_text_fmt(lbl_sp, "sp = %0.*f %s", prec, pid._settings.fpid.setpoint, unit);
            break;
    };

	// input value/sensor
	lv_label_set_text_fmt(lbl_value, "%0.*f %s", prec, pid.input_value(), unit);

	// output bar
	float p = pid.output_value();
	if(isnan(p)) // FIXME: just a hack for now, but bar shows full ifnan
		p = 0;
	lv_bar_set_value(bar_output, p, LV_ANIM_ON); 
};

const void PidPanel::setSelected(bool select) {    lv_obj_set_style_border_color(box, select ? COLOR_BLACK : COLOR_WHITE, 0);};

/*********************************************************************************************************************************/
class GraphPanel
{
	public:
		GraphPanel(lv_obj_t* parent);

		void appendVals(const float val1, const float val2);
		void setScaleY(const lv_chart_axis_t, float min, float max);
		void autoScale(const lv_chart_axis_t axis, const float inc_sp);
		static void draw_lbl_cb(lv_event_t* e);

		lv_obj_t *box;
	    lv_obj_t *chart;
		lv_chart_series_t *ser1, *ser2;
  	    bool redraw = true;
		lv_color_t color_ch1, color_ch2;
};

GraphPanel::GraphPanel(lv_obj_t* parent)
{
	/*Create a chart*/	
	box = lv_obj_create(parent);
	lv_obj_set_style_border_width(box, 0, 0);
	lv_obj_set_style_pad_all(box, 0, 0);
	lv_obj_set_size(box, DISPLAY_WIDTH, DISPLAY_HEIGHT - 78);

    chart = lv_chart_create(box);
    lv_obj_set_size(chart, DISPLAY_WIDTH - 55, DISPLAY_HEIGHT - 80 - 20);
	lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 0);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(chart, GRAPH_YDIVS, GRAPH_XDIVS);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 1, 0, GRAPH_XTICKS, 1, true, 20);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT );

	// Primary = Temp, secondary = Rh
	// setScaleY(LV_CHART_AXIS_PRIMARY_Y, -10*GRAPH_MULTIPLIER, 30*GRAPH_MULTIPLIER);
	// setScaleY(LV_CHART_AXIS_SECONDARY_Y, 0*GRAPH_MULTIPLIER, 100*GRAPH_MULTIPLIER);
	// void lv_chart_set_axis_tick(obj, axis, 						major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size)
    lv_chart_set_axis_tick(	chart,  LV_CHART_AXIS_PRIMARY_Y, 	1,         0,         3,         1,         true,     40);
    lv_chart_set_axis_tick(	chart,  LV_CHART_AXIS_SECONDARY_Y, 	1,         0,         3,         1,         true,     40);

	color_ch1 = COLOR_BLACK;
	color_ch2 = COLOR_BLACK;
	if(pids[0]->input_drv())
		color_ch1 = pids[0]->input_drv()->color1();
	if(pids[1]->input_drv())
		color_ch2 = pids[1]->input_drv()->color1();
	color_ch1 = lv_color_darken(color_ch1, 4);
	color_ch2 = lv_color_darken(color_ch2, 4);

    /*Add two data series*/
	lv_chart_set_point_count(chart, GRAPH_POINTS);
    ser1 = lv_chart_add_series(chart, color_ch1, LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, color_ch2, LV_CHART_AXIS_SECONDARY_Y);
	
	lv_obj_add_event_cb(chart, draw_lbl_cb, LV_EVENT_DRAW_PART_BEGIN, this);
};

void GraphPanel::draw_lbl_cb(lv_event_t* e)
{
	lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) 
		return;

    // if no target buffer, forget about it
    if(!dsc->text)
    {
        WARNING("No target buffer for tick-label?");
        return;
    };

	GraphPanel* me = static_cast<GraphPanel*>(e->user_data);

    // tmp value, don't allocate
    static int this_sec;

// #define GRAPH_TOTAL_SEC         ((int)(GRAPH_POINTS * GRAPH_DELTA_MS / 1000))
// #define GRAPH_SEC_PER_DEV       (GRAPH_TOTAL_SEC / (GRAPH_XTICKS-1))
    uint32_t graph_total_sec = (GRAPH_POINTS * settings.graph_delta) / 1000;
    uint32_t graph_sec_per_div = graph_total_sec / (GRAPH_XTICKS-1);

	// GraphPanel* me = static_cast<GraphPanel*>(e->user_data);
    switch(dsc->id)
    {
        case LV_CHART_AXIS_PRIMARY_X:
            this_sec = graph_sec_per_div * (GRAPH_XTICKS-1-dsc->value);
            lv_snprintf(dsc->text, dsc->text_length, "-%d:%02d", this_sec / 3600, (this_sec % 3600) / 60);
    		return;
        case LV_CHART_AXIS_PRIMARY_Y:
		    dsc->label_dsc->color = me->color_ch1;
            lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value / GRAPH_MULTIPLIER);
            return;
	    case LV_CHART_AXIS_SECONDARY_Y:
		    dsc->label_dsc->color = me->color_ch2;
            lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value / GRAPH_MULTIPLIER);
            return;
        default:
            return;  
    };
};

void GraphPanel::appendVals(const float val1, const float val2)
{
	lv_chart_set_next_value(chart, ser1, isnan(val1) ? LV_CHART_POINT_NONE : val1*GRAPH_MULTIPLIER);
	lv_chart_set_next_value(chart, ser2, isnan(val2) ? LV_CHART_POINT_NONE : val2*GRAPH_MULTIPLIER);

	autoScale(LV_CHART_AXIS_PRIMARY_Y, settings.pid1.fpid.setpoint);
	autoScale(LV_CHART_AXIS_SECONDARY_Y, settings.pid2.fpid.setpoint);
};

void GraphPanel::setScaleY(const lv_chart_axis_t axis, const float miny, const float maxy)
{
	// DBG("axis: %d min/max: %f %f", axis, miny, maxy);

	lv_chart_set_range(chart, axis, miny*GRAPH_MULTIPLIER, maxy*GRAPH_MULTIPLIER);
	// void lv_chart_set_axis_tick(obj, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size)
    lv_chart_set_axis_tick(		chart,  axis, 1,         0,         3,         1,         true,     20);
    // redraw = true;
};

void GraphPanel::autoScale(const lv_chart_axis_t axis, const float inc_sp)
{
	float minyM = inc_sp * GRAPH_MULTIPLIER;
	float maxyM = inc_sp * GRAPH_MULTIPLIER;

	// note: instead of dividing all the points, we multiply the others with GRAPH_MULTIPLIER and then divide once

	// find min/max
	int cnt = lv_chart_get_point_count(chart);
	lv_chart_series_t *ser = axis == LV_CHART_AXIS_PRIMARY_Y ? ser1 : ser2;
	while(cnt--)
	{
        if(ser->y_points[cnt] == LV_CHART_POINT_NONE)
            continue;

        float point = static_cast<float>(ser->y_points[cnt]);

		minyM = min(minyM, point);
		maxyM = max(maxyM, point);
	};
	minyM = floor(minyM/(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER)) *(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER);
	maxyM = ceil(maxyM/(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER)) *(GRAPH_SCALE_ROUND*GRAPH_MULTIPLIER);

	// setScaleY(axis, (minyM/GRAPH_MULTIPLIER) - GRAPH_SCALE_ROOUND, (maxyM/GRAPH_MULTIPLIER) + GRAPH_SCALE_ROOUND);
	lv_chart_set_range(chart, axis, minyM, maxyM);
};

/*********************************************************************************************************************************/
MainScreen::MainScreen(SooghGUI& g) : Screen(g)
{	
	pw1 = new PidPanel(_screen, 1, *(pids[0]));
	pw2 = new PidPanel(_screen, 2, *(pids[1]));
	lv_obj_align_to(pw2->box, pw1->box, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

	gw = new GraphPanel(_screen);
	lv_obj_align(gw->box, LV_ALIGN_BOTTOM_MID, 0, 0);

	if(rtc_available())
	{
		clk_lbl = lv_label_create(_screen);
		{
			lv_obj_align_to(clk_lbl, _screen, LV_ALIGN_TOP_MID, 0, 0);
			lv_label_set_text(clk_lbl, "--:--");
			lv_obj_set_style_border_width(clk_lbl, 1, 0);
		};
	};
};

MainScreen::~MainScreen()
{
	lv_obj_del(clk_lbl); clk_lbl = nullptr;
	delete(pw1);
	delete(pw2);
	delete(gw);
};

void MainScreen::loop()
{
	time_t now = millis();
	if(now < _next_update)
		return;

	_next_update = now + MAIN_LOOP_MS;

	pw1->update();
	pw2->update();

	if(clk_lbl != nullptr)
	{
		rtc_read();
		lv_label_set_text_fmt(clk_lbl, "%02d:%02d", today.tm_hour, today.tm_min);
	};

	if(now < _next_chart)
		return;

	gw->appendVals(
		pids[0]->input_value(), 
		pids[1]->input_value()
		);

	_next_chart = now + settings.graph_delta;
    
    return;
};

bool MainScreen::handle(soogh_event_t key)
{
	switch(key)
	{
		case KEY_A_LONG:
			settings.pid1.active = !settings.pid1.active;
			return true;
		case KEY_C_LONG:
			settings.pid2.active = !settings.pid2.active;
			return true;
		case KEY_ABC_LONG:
			settings.expert_mode = true;
			// fallthrough
		case KEY_B_PRESSED:
		    gui.pushScreen(std::make_shared<MenuScreen>(gui));
			return true;
		default: break;
	};
	return true;
};
