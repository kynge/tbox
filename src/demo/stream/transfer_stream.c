/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_transfer_stream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STATE_OK) percent = 100;

	// trace
	tb_trace_i("save: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_state_cstr(state));

	// ok
	return tb_true;
}
static tb_pointer_t tb_demo_transfer_stream_loop(tb_cpointer_t data)
{
	// the aicp
	tb_aicp_t* aicp = (tb_aicp_t*)data;

	// done loop
	if (aicp) tb_aicp_loop(aicp);

	// exit it
	tb_thread_return(tb_null);
	return tb_null;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_transfer_stream_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_aicp_t* 		aicp = tb_null;
	tb_handle_t 	tstream = tb_null;
	tb_async_stream_t* 	istream = tb_null;
	tb_basic_stream_t* 	ostream = tb_null;
	do
	{
		// init aicp
		aicp = tb_aicp_init(2);
		tb_assert_and_check_break(aicp);

		// init tstream
#if 1
		tstream = tb_transfer_stream_init_uu(aicp, argv[1], argv[2], 0);
#else
		istream = tb_async_stream_init_from_url(aicp, argv[1]);
		ostream = tb_basic_stream_init_from_file(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
		tstream = tb_transfer_stream_init_ag(istream, ostream, -1);
#endif
		tb_assert_and_check_break(tstream);

		// init loop
		if (!tb_thread_init(tb_null, tb_demo_transfer_stream_loop, aicp, 0)) break;

		// limit rate
		tb_transfer_stream_limitrate(tstream, argv[3]? tb_atoi(argv[3]) : 0);

		// trace
		tb_trace_i("save: ..");

		// open and save tstream
		if (!tb_transfer_stream_osave(tstream, tb_demo_transfer_stream_save_func, tb_null)) break;

		// wait
		getchar();

		// trace
		tb_trace_i("pause: ..");

		// pause tstream
		tb_transfer_stream_pause(tstream);

		// wait
		getchar();

		// trace
		tb_trace_i("resume: ..");

		// start tstream
		if (!tb_transfer_stream_resume(tstream)) break;

		// wait
		getchar();

		// trace
		tb_trace_i("save: ..");

		// open and save tstream
		if (!tb_transfer_stream_osave(tstream, tb_demo_transfer_stream_save_func, tb_null)) break;

		// wait
		getchar();

	} while (0);

	// trace
	tb_trace_i("exit: ..");

	// exit tstream
	if (tstream) tb_transfer_stream_exit(tstream, tb_false);
	tstream = tb_null;

	// exit istream
	if (istream) tb_async_stream_exit(istream, tb_false);
	istream = tb_null;

	// exit ostream
	if (ostream) tb_basic_stream_exit(ostream);
	ostream = tb_null;

	// exit aicp
	if (aicp) tb_aicp_exit(aicp);
	aicp = tb_null;
	return 0;
}
