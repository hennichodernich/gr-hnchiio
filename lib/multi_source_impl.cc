/* -*- c++ -*- */
/* 
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include "multi_source_impl.h"

#include <cstdio>
#include <fstream>
#include <string>

namespace gr {
  namespace hnchiio {

    multi_source::sptr
    multi_source::make(const std::string &uri, const std::string &device,
		    const std::vector<std::string> &channels,
		    const std::string &device_phy1,
		    const std::vector<std::string> &params1,
		    const std::string &device_phy2,
		    const std::vector<std::string> &params2,            
		    unsigned int buffer_size, unsigned int decimation)
    {
      return gnuradio::get_initial_sptr
        (new multi_source_impl(multi_source_impl::get_context(uri), true,
				device, channels, device_phy1,
				params1, device_phy2,
				params2, buffer_size, decimation));
    }

    multi_source::sptr
    multi_source::make_from(struct iio_context *ctx, const std::string &device,
		    const std::vector<std::string> &channels,
		    const std::string &device_phy1,
		    const std::vector<std::string> &params1,
		    const std::string &device_phy2,
		    const std::vector<std::string> &params2,
		    unsigned int buffer_size, unsigned int decimation)
    {
      return gnuradio::get_initial_sptr
        (new multi_source_impl(ctx, false, device, channels, device_phy1,
				params1, device_phy2,
				params2, buffer_size, decimation));
    }

    void multi_source_impl::set_params(struct iio_device *phy,
		    const std::vector<std::string> &params)
    {
	    for (std::vector<std::string>::const_iterator it = params.begin();
			    it != params.end(); ++it) {
		    struct iio_channel *chn = NULL;
		    const char *attr = NULL;
		    size_t pos;
		    int ret;

		    pos = it->find('=');
		    if (pos == std::string::npos) {
			    std::cerr << "Misformed line: " << *it << std::endl;
			    continue;
		    }

		    std::string key = it->substr(0, pos);
		    std::string val = it->substr(pos + 1, std::string::npos);

		    ret = iio_device_identify_filename(phy,
				    key.c_str(), &chn, &attr);
		    if (ret) {
			    std::cerr << "Parameter not recognized: "
				    << key << std::endl;
			    continue;
		    }

		    if (chn)
			    ret = iio_channel_attr_write(chn,
					    attr, val.c_str());
		    else if (iio_device_find_attr(phy, attr))
			    ret = iio_device_attr_write(phy, attr, val.c_str());
		    else
			    ret = iio_device_debug_attr_write(phy,
					    attr, val.c_str());
		    if (ret < 0) {
			    std::cerr << "Unable to write attribute " << key
				    <<  ": " << ret << std::endl;
		    }
	    }
    }

    void multi_source_impl::set_params0(const std::vector<std::string> &params)
    {
	    set_params(this->dev, params);
    }
    
    void multi_source_impl::set_params1(const std::vector<std::string> &params)
    {
	    set_params(this->phy1, params);
    }
    
    void multi_source_impl::set_params2(const std::vector<std::string> &params)
    {
	    set_params(this->phy2, params);
    }

    void multi_source_impl::set_buffer_size(unsigned int _buffer_size)
    {
	    boost::unique_lock<boost::mutex> lock(iio_mutex);

	    if (buf && this->buffer_size != _buffer_size) {
		iio_buffer_destroy(buf);

		buf = iio_device_create_buffer(dev, _buffer_size, false);
		if (!buf)
			throw std::runtime_error("Unable to create buffer!\n");
	    }

	    this->buffer_size = _buffer_size;
    }

    void multi_source_impl::set_timeout_ms(unsigned long _timeout)
    {
	    this->timeout = _timeout;
    }

    struct iio_context * multi_source_impl::get_context(
		    const std::string &uri)
    {
	    struct iio_context *ctx;
	    std::map<std::string,struct iio_context *>::iterator it;

	    // Check if we have a context with the same URI open
	    if (!contexts.empty()){
	    	it = contexts.find(uri);
	        if (it != contexts.end()) return it->second;
	    }

	    if (uri.empty()) {
		    ctx = iio_create_default_context();
		    if (!ctx)
			    ctx = iio_create_network_context(NULL);
	    } else {
		    ctx = iio_create_context_from_uri(uri.c_str());

		    /* Stay compatible with old graphs, by accepting an
		     * IP/hostname instead of an URI */
		    if (!ctx)
			    ctx = iio_create_network_context(uri.c_str());
	    }
	    // Save context info for future checks
	    contexts[uri] = ctx;

	    return ctx;
    }

    /*
     * The private constructor
     */
    multi_source_impl::multi_source_impl(struct iio_context *ctx,
		    bool destroy_ctx, const std::string &device,
		    const std::vector<std::string> &channels,
		    const std::string &device_phy1,
		    const std::vector<std::string> &params1,
		    const std::string &device_phy2,
		    const std::vector<std::string> &params2,
		    unsigned int buffer_size, unsigned int decimation)
      : gr::sync_block("multi_source",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, -1, sizeof(short))),
        port_id(pmt::mp("msg")),
        timeout(100),
        ctx(ctx), buf(NULL),
        buffer_size(buffer_size),
        decimation(decimation),
        destroy_ctx(destroy_ctx)
    {
	    unsigned int nb_channels, i;

	    if (!ctx)
		    throw std::runtime_error("Unable to create context");

	    dev = iio_context_find_device(ctx, device.c_str());
	    phy1 = iio_context_find_device(ctx, device_phy1.c_str());
        phy2 = iio_context_find_device(ctx, device_phy2.c_str());
	    if (!dev || !phy1 || !phy2) {
		    if (destroy_ctx)
			    iio_context_destroy(ctx);
		    throw std::runtime_error("Device not found");
	    }

	    /* First disable all channels */
	    nb_channels = iio_device_get_channels_count(dev);
	    for (i = 0; i < nb_channels; i++)
		    iio_channel_disable(iio_device_get_channel(dev, i));

	    if (channels.empty()) {
		    for (i = 0; i < nb_channels; i++) {
			    struct iio_channel *chn =
				    iio_device_get_channel(dev, i);

			    iio_channel_enable(chn);
			    channel_list.push_back(chn);
		    }
	    } else {
		    for (std::vector<std::string>::const_iterator it =
				    channels.begin();
				    it != channels.end(); ++it) {
			    struct iio_channel *chn =
				    iio_device_find_channel(dev,
						    it->c_str(), false);
			    if (!chn) {
				    if (destroy_ctx)
					    iio_context_destroy(ctx);
				    throw std::runtime_error(
						    "Channel not found");
			    }

			    iio_channel_enable(chn);
			    channel_list.push_back(chn);
		    }
	    }

	    set_params1(params1);
        set_params2(params2);
	    set_output_multiple(0x400);

	    message_port_register_out(port_id);
    }

    /*
     * Our virtual destructor.
     */
    multi_source_impl::~multi_source_impl()
    {
	    if (destroy_ctx)
		    iio_context_destroy(ctx);
    }

    void
    multi_source_impl::channel_read(const struct iio_channel *chn,
		    void *dst, size_t len)
    {
	uintptr_t src_ptr, dst_ptr = (uintptr_t) dst, end = dst_ptr + len;
	unsigned int length = iio_channel_get_data_format(chn)->length / 8;
	uintptr_t buf_end = (uintptr_t) iio_buffer_end(buf);
	ptrdiff_t buf_step = iio_buffer_step(buf) * (decimation + 1);

	for (src_ptr = (uintptr_t) iio_buffer_first(buf, chn) + byte_offset;
			src_ptr < buf_end && dst_ptr + length <= end;
			src_ptr += buf_step, dst_ptr += length)
		iio_channel_convert(chn,
				(void *) dst_ptr, (const void *) src_ptr);
    }

    void
    multi_source_impl::refill_thread()
    {
	boost::unique_lock<boost::mutex> lock(iio_mutex);
	ssize_t ret;

	for (;;) {
		while (!please_refill_buffer)
			iio_cond.wait(lock);

		please_refill_buffer = false;

		lock.unlock();
		ret = iio_buffer_refill(buf);
		lock.lock();

		if (ret < 0)
			break;

		items_in_buffer = (unsigned long) ret / iio_buffer_step(buf);
		byte_offset = 0;

		iio_cond2.notify_all();
	}

	/* -EBADF happens when the buffer is cancelled */
	if (ret != -EBADF) {

		char buf[256];
		iio_strerror(-ret, buf, sizeof(buf));
		std::string error(buf);

		std::cerr << "Unable to refill buffer: " << error << std::endl;
	}

	thread_stopped = true;
	iio_cond2.notify_all();
    }

    int
    multi_source_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
	boost::unique_lock<boost::mutex> lock(iio_mutex);

	if (thread_stopped)
		return -1; /* EOF */

	/* No items in buffer -> ask for a refill */
	if (!please_refill_buffer && !items_in_buffer) {
		please_refill_buffer = true;
		iio_cond.notify_all();
	}

	while (please_refill_buffer) {
		bool fast_enough = iio_cond2.timed_wait(lock,
				boost::posix_time::milliseconds(timeout));
		if (thread_stopped)
			return -1; /* EOF */

		if (!fast_enough) {
			message_port_pub(port_id, pmt::mp("timeout"));

			/* GNU Radio won't call our block anytime soon if the
			 * work() function returns 0; it can take up to 500ms
			 * before it gets called again. To avoid that, we make
			 * this block send itself a dummy message on its input
			 * system message port, so that the scheduler calls us
			 * again promptly. */
#ifdef GR_VERSION_3_7_OR_LESS
			pmt::pmt_t payload = pmt::from_long(0);
#else
			pmt::pmt_t payload = pmt::from_bool(false);
#endif
			pmt::pmt_t msg = pmt::cons(pmt::mp("done"), payload);
			post(pmt::mp("system"), msg);
			return 0;
		}
	}

	unsigned long items = std::min(items_in_buffer,
			(unsigned long) noutput_items);

	for (unsigned int i = 0; i < output_items.size(); i++) {
		channel_read(channel_list[i], output_items[i],
				items * sizeof(short));

		if (!byte_offset) {
			tag_t tag;
			tag.value = pmt::from_long(items_in_buffer);
			tag.offset = sample_counter;
			tag.key = pmt::intern("buffer_start");
			tag.srcid = alias_pmt();

			add_item_tag(i, tag);
		}
	}

	sample_counter += items;
	items_in_buffer -= items;
	byte_offset += items * iio_buffer_step(buf);

	return (int) items;
    }

    bool multi_source_impl::start()
    {
	boost::unique_lock<boost::mutex> lock(iio_mutex);

	sample_counter = 0;
	items_in_buffer = 0;
	please_refill_buffer = false;
	thread_stopped = false;

	buf = iio_device_create_buffer(dev, buffer_size, false);

	if (buf) {
		refill_thd = boost::thread(
				&multi_source_impl::refill_thread, this);
	} else {
		throw std::runtime_error("Unable to create buffer!\n");
	}

	return !!buf;
    }

    bool multi_source_impl::stop()
    {
	    if (buf)
		    iio_buffer_cancel(buf);

	    boost::unique_lock<boost::mutex> lock(iio_mutex);
	    please_refill_buffer = true;
	    iio_cond.notify_all();
	    lock.unlock();

	    refill_thd.join();

	    if (buf) {
		    iio_buffer_destroy(buf);
		    buf = NULL;
	    }

	    return true;
    }

    bool multi_source_impl::load_fir_filter(
		    std::string &filter, struct iio_device *phy)
    {
	if (filter.empty() || !iio_device_find_attr(phy, "filter_fir_config"))
		return false;

	std::ifstream ifs(filter.c_str(), std::ifstream::binary);
	if (!ifs)
		return false;

	/* Here, we verify that the filter file contains data for both RX+TX. */
	{
		char buf[256];

		do {
			ifs.getline(buf, sizeof(buf));
		} while (!(buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9')));

		std::string line(buf);
		if (line.find(',') == std::string::npos)
			throw std::runtime_error("Incompatible filter file");
	}

	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	char *buffer = new char [length];

	ifs.read(buffer, length);
	ifs.close();

	int ret = iio_device_attr_write_raw(phy,
			"filter_fir_config", buffer, length);

	delete[] buffer;
	return ret > 0;
    }

  } /* namespace hnchiio */
} /* namespace gr */
